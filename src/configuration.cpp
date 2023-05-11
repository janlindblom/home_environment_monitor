// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "configuration.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <BTstackLib.h>
#include <LittleFS.h>
#include <string.h>

#include <vector>

#include "configuration_types.h"

bool   _configured           = false;
bool   _configuration_loaded = false;
Config _config;

/**
 * This is from the initial implementation since BD_ADDR in arduino-pico does
 * not (yet) have a constructor that takes a char* as input.
 */
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes,
                int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base); // Convert byte
    str      = strchr(str, sep);         // Find next separator
    if (str == NULL || *str == '\0') {
      break; // No more separators, exit
    }
    str++; // Point to next character after separator
  }
}

Config get_config() {
  return _config;
}

void load_configuration() {
  StaticJsonDocument<2048> doc;
  noInterrupts();
  File config_file = LittleFS.open(json_config, "r");

  DeserializationError error = deserializeJson(doc, config_file);
  config_file.close();
  interrupts();

  if (error) {
    Serial.println(F("Failed to read file, using default configuration"));
    Serial.println(error.c_str());
    _configuration_loaded = false;
  } else {
    _configuration_loaded = true;
  }

  for (JsonPair network : doc["networks"].as<JsonObject>()) {
    const char* network_key = network.key().c_str(); // "primary", "secondary"

    const char* network_value_ssid     = network.value()["ssid"];
    const char* network_value_password = network.value()["password"];

    char* ssid     = new char[strlen(network_value_ssid) + 1];
    char* password = new char[strlen(network_value_password) + 1];

    strcpy(ssid, network_value_ssid);
    strcpy(password, network_value_password);

    if (!strcmp(network_key, "primary")) {
      _config.networks.primary.ssid     = ssid;
      _config.networks.primary.password = password;
    } else if (!strcmp(network_key, "secondary")) {
      _config.networks.secondary.ssid     = ssid;
      _config.networks.secondary.password = password;
    }
  }

  const char* tz   = doc["timezone"] | "GMT";
  _config.timezone = std::string(tz);

  JsonObject location        = doc["location"];
  _config.location.latitude  = location["latitude"];
  _config.location.longitude = location["longitude"];
  _config.location.tz_offset = location["tz_offset"];
  _config.location.elevation = location["elevation"];

  uint8_t device_number = 0;
  for (JsonObject ruuvi_device : doc["ruuvi"]["devices"].as<JsonArray>()) {
    ruuvi_device_t device;

    const char* device_name = ruuvi_device["name"];
    device.device_name      = std::string(device_name);
    strncpy(device.name, device.device_name.c_str(), sizeof(device.name));

    const char* device_placement = ruuvi_device["placement"];
    device.device_placement      = std::string(device_placement);
    strncpy(device.placement, device.device_placement.c_str(),
            sizeof(device.placement));

    strncpy(device.address, ruuvi_device["address"], sizeof(device.address));

    uint8_t addr[6];
    parseBytes(ruuvi_device["address"], ':', addr, 6, 16);
    // device.bt_addr = BD_ADDR(addr);
    // This depends on PR#1440 to be merged into arduino-pico otherwise, use the
    // line above.
    device.bt_addr = BD_ADDR(device.address);
    memcpy(device.addr, addr, sizeof(device.addr));
    _config.ruuvi.devices[device_number] = device;
    _config.ruuvi.ruuvi_devices.push_back(device);
    device_number++;
  }
  _config.ruuvi.ruuvi_devices.shrink_to_fit();
  _config.ruuvi.count = device_number;

  if (error) {
    _configured = false;
  } else {
    _configured = true;
  }
}

bool configured() {
  return _configured;
}

bool configuration_loaded() {
  return _configuration_loaded;
}
