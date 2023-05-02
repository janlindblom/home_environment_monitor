// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "configuration.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <string.h>

#include "configuration_types.h"

bool   _configured           = false;
bool   _configuration_loaded = false;
Config _config;

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

    const char* network_value_ssid     = network.value()["ssid"] | "";
    const char* network_value_password = network.value()["password"] | "";

    if (!strcmp(network_key, "primary")) {
      strncpy(_config.networks.primary.ssid, network_value_ssid,
              sizeof(_config.networks.primary.ssid));
      strncpy(_config.networks.primary.password, network_value_password,
              sizeof(_config.networks.primary.password));
    } else if (!strcmp(network_key, "secondary")) {
      strncpy(_config.networks.secondary.ssid, network_value_ssid,
              sizeof(_config.networks.secondary.ssid));
      strncpy(_config.networks.secondary.password, network_value_password,
              sizeof(_config.networks.secondary.password));
    }
  }

  strncpy(_config.timezone, doc["timezone"] | "GMT", sizeof(_config.timezone));

  JsonObject location        = doc["location"];
  _config.location.latitude  = location["latitude"];
  _config.location.longitude = location["longitude"];
  _config.location.tz_offset = location["tz_offset"];
  _config.location.elevation = location["elevation"];

  uint8_t device_number = 0;
  for (JsonObject ruuvi_device : doc["ruuvi"]["devices"].as<JsonArray>()) {
    ruuvi_device_t device;
    strncpy(device.name, ruuvi_device["name"], sizeof(device.name));
    strncpy(device.placement, ruuvi_device["placement"],
            sizeof(device.placement));
    strncpy(device.address, ruuvi_device["address"], sizeof(device.address));

    uint8_t addr[6];
    parseBytes(ruuvi_device["address"], ':', addr, 6, 16);
    memcpy(device.addr, addr, sizeof(device.addr));
    _config.ruuvi.devices[device_number] = device;
    device_number++;
  }
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
