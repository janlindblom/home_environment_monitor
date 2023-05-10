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

#include "climate.h"
#include "common.h"
#include "configuration.h"
#include "configuration_types.h"
#include "forecast.h"
#include "network_time.h"
#include "ruuvi.h"
#include "ruuvi_types.h"
#include "splash_logo.h"
#include "system.h"
#include "wireless.h"

uint32_t display_timer = 0;

U8G2_SH1107_64X128_F_HW_I2C u8g2(U8G2_R3);
//U8G2_SSD1327_WS_128X128_F_HW_I2C u8g2(U8G2_R3);

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

void load_config_file();

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
  setup_ruuvi_devices(config);

  u8g2.setI2CAddress(I2C_ADDRESS << 1);
  u8g2.begin();
  u8g2.enableUTF8Print();
  setup_backlight();

  u8g2.clearBuffer();
  u8g2.drawXBM((u8g2.getDisplayWidth() >> 1) - (splash_logo_width >> 1),
               (u8g2.getDisplayHeight() >> 1) - (splash_logo_height >> 1),
               splash_logo_width, splash_logo_height, splash_logo_bits);
  u8g2.sendBuffer();

  control_backlight(u8g2);
  delay(1000);

  u8g2.clearBuffer();
  u8g2.sendBuffer();

  Serial.println(F("Connecting to WiFi..."));
  connect_network(config);
  Serial.println(F("WiFi connection configured."));
  // pir_init();
}

void load_config_file() {
  if (_filesystem_safe) {
    Serial.println(F("Reading configuration..."));
    load_configuration();
    Serial.println(F("Configuration loaded."));
    config = get_config();
  }
}

/**
 * Main program loop.
 */
void loop() {
  delay(250);

  if (configuration_loaded()) {
    control_wireless(config, advertisementCallback);
  }

  if (bluetooth_configured()) {
    control_bluetooth_scanning();
  }

  u8g2.setFont(u8g2_font_helvR08_tf);
  u8g2.clearBuffer();

  if (configured()) {
    print_wifi_status(u8g2);
    print_time(u8g2, config);
  } else {
    load_config_file();
  }
  if (configured() && bluetooth_configured()) {
    print_bluetooth_status(u8g2);
    if (ruuvi_devices_configured()) {
      process_pressure(ruuvi_readings(), ruuvi_outdoor_sensor(), config);
      print_climate(u8g2, ruuvi_readings(), ruuvi_outdoor_sensor(),
                    config.ruuvi.count);
      print_forecast_icon(u8g2, config);
    } else {
      setup_ruuvi_devices(config);
    }
  }

  u8g2.sendBuffer();
  control_backlight(u8g2);
}

/**
 * Callback function for BLE Advertisements. Called when a devices is
 * discovered during BLE device scan.
 */
void advertisementCallback(BLEAdvertisement* adv) {
  char adv_addr[strlen(adv->getBdAddr()->getAddressString())];
  strcpy(adv_addr, adv->getBdAddr()->getAddressString());
  if (adv->isIBeacon()) {
    Serial.print("iBeacon found ");
    Serial.print(adv->getBdAddr()->getAddressString());
    Serial.print(", RSSI ");
    Serial.print(adv->getRssi());
    Serial.print(", UUID ");
    Serial.print(adv->getIBeaconUUID()->getUuidString());
    Serial.print(", MajorID ");
    Serial.print(adv->getIBeaconMajorID());
    Serial.print(", MinorID ");
    Serial.print(adv->getIBecaonMinorID());
    Serial.print(", Measured Power ");
    Serial.println(adv->getiBeaconMeasuredPower());
  } else {
    if (!configured() || !ruuvi_devices_configured()) {
      return;
    }

    for (uint8_t i = 0; i < config.ruuvi.count; i++) {
      if (!strcmp(adv_addr, ruuvi_devices()[i].getAddressString())) {
        uint8_t data[LE_ADVERTISING_DATA_SIZE];
        memcpy(data, adv->getAdvData(), LE_ADVERTISING_DATA_SIZE);
        if (data[0] != 0x11) {
          time_t       now   = time(nullptr);
          ruuvi_data_t rdata = make_ruuvi_data(data);
          store_ruuvi_reading(i, rdata);
          if ((ruuvi_reading_times()[i] == 0) ||
              difftime(now, ruuvi_reading_times()[i]) >= 360.0f) {
            Serial.println(
                F("Six minutes since last logged reading, saving..."));
            store_ruuvi_reading_time(i, now);
            Serial.print(F("Logging Ruuvi device: "));
            Serial.println(config.ruuvi.devices[i].name);
            Serial.print(F("Current pressure trend: "));
            Serial.println(pressure_trend());
            Serial.print(F("Zambretti trend: "));
            Serial.println(current_trend(pressure_trend()).baro_trend);
            Serial.print(F("Zambretti indication: "));
            Serial.println(current_trend(pressure_trend()).indication);
            if (average_pressure() > 0) {
              zambretti_forecast f = get_forecast(config);
              Serial.print(F("Forecast: "));
              Serial.print(f.forecast);
              Serial.print(F(": "));
              Serial.println(f.description);
            }
          }
        }
      }
    }
  }
}
