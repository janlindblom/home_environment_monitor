// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "wireless.h"

#include <Arduino.h>
#include <BTstackLib.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <WiFi.h>

#include <string>

#include "common.h"
#include "configuration.h"
#include "network_time.h"

const uint16_t signal_strength[] = {57890, 57889, 57888, 57888, 57887};

bool _wifi_ap_configured    = false;
bool _network_connected     = false;
bool _network_setup_running = false;

bool _bluetooth_scanning    = false;
bool _bluetooth_configured  = false;
bool _ble_advertisements    = false;
bool _bluetooth_configuring = false;

uint32_t comms_timer = 0;

WiFiMulti multi;

/**
 * Controls the wireless hardware state according to the current state of the
 * system.
 *
 * \param configuration the parsed config file
 * \param callback the function called when Bluetooth discovers something
 */
void control_wireless(Config configuration,
                      void (*callback)(BLEAdvertisement* bleAdvertisement)) {
  if (!_network_connected && !network_time_set()) {
    Serial.println(F("No network connection, trying to connect."));
    connect_network(configuration);
  } else if (_network_connected && !network_time_set()) {
    configure_network_time(configuration);
  } else if (network_time_set() && _network_connected) {
    Serial.println(F("No more need for network, disconnecting..."));
    disconnect_network();
  } else if ((!_network_connected && network_time_set()) &&
             !_bluetooth_configured) {
    Serial.println(
        F("Time is set and network disconnected, setting up Bluetooth..."));
    configure_bluetooth(callback);
  }
}

//// WiFi section

/**
 * Connects to WiFi.
 */
void connect_network(Config configuration) {
  if (!configured() || _network_setup_running || WiFi.connected()) {
    return;
  }

  _network_setup_running = true;

  WiFi.setHostname("envmon");

  if (!_wifi_ap_configured) {
    if (configuration.networks.secondary.ssid.length() > 0) {
      if (multi.addAP(configuration.networks.secondary.ssid.c_str(),
                      configuration.networks.secondary.password.c_str())) {
        _wifi_ap_configured = true;
      }
    }
    if (configuration.networks.primary.ssid.length() > 0) {
      if (multi.addAP(configuration.networks.primary.ssid.c_str(),
                      configuration.networks.primary.password.c_str())) {
        _wifi_ap_configured = true;
      }
    }
  }

  _network_connected     = (multi.run() == WL_CONNECTED);
  _network_setup_running = false;
}

/**
 * Disconnects from WiFi.
 */
void disconnect_network() {
  if (!_network_setup_running && _network_connected) {
    if (WiFi.disconnect(true) == WL_DISCONNECTED) {
      _network_connected = false;
    }
  }
}

/**
 * Rate the current WiFi signal from UNUSABLE to AMAZING.
 *
 * \param rssi the RSSI value for the connection
 */
uint8_t wifi_signal_rating(int rssi) {
  if (rssi >= -30) {
    return WiFiSignal::AMAZING;
  } else if (rssi >= -67) {
    return WiFiSignal::GREAT;
  } else if (rssi >= -70) {
    return WiFiSignal::GOOD;
  } else if (rssi >= -80) {
    return WiFiSignal::BAD;
  }
  return WiFiSignal::UNUSABLE;
}

/**
 * Show the current WiFi status on the OLED.
 *
 * \param u8g2 the OLED display
 */
void print_wifi_status(U8G2 u8g2) {
  if (_network_connected) {
    u8g2.setFont(u8g2_font_siji_t_6x10);
    uint16_t signal_icon = signal_strength[wifi_signal_rating(WiFi.RSSI())];
    u8g2.drawGlyph(u8g2.getDisplayWidth() - u8g2.getMaxCharWidth() - 1,
                   u8g2.getDisplayHeight() - 1, signal_icon);
  }
  uint8_t icon_width = u8g2.getMaxCharWidth();
  if (!_bluetooth_configured && !_bluetooth_scanning &&
      !_bluetooth_configuring) {
    u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
    u8g2.drawGlyph(
        u8g2.getDisplayWidth() - u8g2.getMaxCharWidth() - icon_width - 1,
        u8g2.getDisplayHeight() - 1, 72);
  }
}

bool wifi_ap_configured() {
  return _wifi_ap_configured;
}

bool network_connected() {
  return (_network_connected && WiFi.connected());
}

bool network_disconnected() {
  return !network_connected();
}

bool network_setup_running() {
  return _network_setup_running;
}

//// Bluetooth section

/**
 * Configure the Bluetooth connection.
 *
 * \param callback the callback function for BLE advertisements
 */
void configure_bluetooth(void (*callback)(BLEAdvertisement* bleAdvertisement)) {
  if (_bluetooth_configuring || _bluetooth_configured) {
    return;
  }

  Serial.println(F("Configuring Bluetooth..."));
  _bluetooth_configuring = true;
  BTstack.setBLEAdvertisementCallback(callback);
  BTstack.setup();
  _bluetooth_configured  = true;
  _bluetooth_configuring = false;
  Serial.println(F("Bluetooth configured."));
}

/**
 * Show Bluetooth status on the OLED.
 *
 * \param u8g2 the OLED display
 */
void print_bluetooth_status(U8G2 u8g2) {
  if (_bluetooth_scanning && !_network_connected) {
    u8g2.setFont(u8g2_font_siji_t_6x10);
    u8g2.drawGlyph(u8g2.getDisplayWidth() - u8g2.getMaxCharWidth() - 1,
                   u8g2.getDisplayHeight() - 1, 57355);
  }
}

/**
 * Control the Bluetooth scanning state.
 */
void control_bluetooth_scanning() {
  if (!_network_connected && _bluetooth_configured) {
    // Scan for 10 seconds then sleep for 10 seconds
    if ((millis() - comms_timer) >= 10000) {
      if (_bluetooth_scanning) {
        ble_stop_scanning();
      } else {
        ble_start_scanning();
      }
    }
  }
}

/**
 * Start scanning for BLE devices.
 */
void ble_start_scanning() {
  comms_timer         = millis();
  _bluetooth_scanning = true;
  BTstack.bleStartScanning();
}

/**
 * Stop scanning for BLE devices.
 */
void ble_stop_scanning() {
  comms_timer         = millis();
  _bluetooth_scanning = false;
  BTstack.bleStopScanning();
}

bool bluetooth_configured() {
  return _bluetooth_configured;
}

bool bluetooth_configuring() {
  return _bluetooth_configuring;
}

bool bluetooth_scanning() {
  return _bluetooth_scanning;
}
