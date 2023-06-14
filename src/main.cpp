// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <BTstackLib.h>
#include <LittleFS.h>
#include <SPI.h>
#include <SingleFileDrive.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sunset.h>
#include <sys/time.h>
#include <time.h>

#include <iterator>
#include <string>
#include <vector>

#include "climate.h"
#include "common.h"
#include "configuration.h"
#include "configuration_types.h"
#include "forecast.h"
#include "network_time.h"
#include "ruuvi.h"
#include "splash_logo.h"
#include "system.h"
#include "wireless.h"

uint32_t display_timer = 0;

U8G2_SH1107_64X128_F_HW_I2C u8g2(U8G2_R3);
// U8G2_SSD1327_WS_128X128_F_HW_I2C u8g2(U8G2_R3);

Config config;

bool _filesystem_safe = true;

void myPlugCB(uint32_t data) {
  _filesystem_safe = false;
}

void myUnplugCB(uint32_t data) {
  _filesystem_safe = true;
}

void myDeleteCB(uint32_t data) {
  // Maybe LittleFS.remove("myfile.txt")?  or do nothing
}

/**
 * Main setup routine.
 */
void setup() {
  LittleFS.begin();
  /* singleFileDrive.onPlug(myPlugCB);
  singleFileDrive.onUnplug(myUnplugCB);
  singleFileDrive.onDelete(myDeleteCB);
  singleFileDrive.begin("config.json", "config.json"); */

  if (!Serial) {
    Serial.begin(115200);
  }
  delay(10000);
  load_config_file();
  config = get_config();
  setup_ruuvi_devices();

  u8g2.setI2CAddress(I2C_ADDRESS << 1);
  u8g2.begin();
  u8g2.enableUTF8Print();
  setup_backlight();

  u8g2.clearBuffer();
  u8g2.drawXBM((u8g2.getDisplayWidth() >> 1) - (splash_logo_width >> 1),
               (u8g2.getDisplayHeight() >> 1) - (splash_logo_height >> 1), splash_logo_width, splash_logo_height,
               splash_logo_bits);
  u8g2.sendBuffer();

  control_backlight();
  delay(1000);

  u8g2.clearBuffer();
  u8g2.sendBuffer();

  Serial.println(F("Connecting to WiFi..."));
  connect_network();
  Serial.println(F("WiFi connection configured."));
  // pir_init();
}

/**
 * Main program loop.
 */
void loop() {
  delay(250);

  if (watchdog_running()) {
    // Make sure we feed the watchdog if it's running.
    feed_watchdog();
  }

  if (configuration_loaded()) {
    control_wireless();
  }

  if (bluetooth_configured()) {
    control_bluetooth_scanning();
    // Once BLE is configured, also start the watchdog if not running.
    if (!watchdog_running()) {
      start_watchdog();
    }
  }

  u8g2.setFont(u8g2_font_helvR08_tf);
  u8g2.clearBuffer();

  if (configured()) {
    print_wifi_status();
    print_time();
  } else {
    load_config_file();
  }

  if (configured() && bluetooth_configured()) {
    print_bluetooth_status();
    if (ruuvi_devices_configured()) {
      process_pressure();
      print_climate();
      print_forecast_icon();
    } else {
      setup_ruuvi_devices();
    }
  }

  u8g2.sendBuffer();
  control_backlight();
}

/*
void setup1() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop1() {
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}
*/

bool is_filesystem_safe() {
  return _filesystem_safe;
}

U8G2 get_display() {
  return u8g2;
}
