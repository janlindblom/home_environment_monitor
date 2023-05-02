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

#include "arduino_secrets.h"
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

#define RUUVI_DEVICES 3

BD_ADDR*      ruuvi_devices;
bool*         ruuvi_outdoor_sensor;
ruuvi_data_t* ruuvi_readings;
time_t*       ruuvi_reading_time;

uint32_t display_timer = 0;

U8G2_SH1107_64X128_F_HW_I2C u8g2(U8G2_R1);

Config config;

bool filesystem_safe = true;

void myPlugCB(uint32_t data) {
  filesystem_safe = false;
}

void myUnplugCB(uint32_t data) {
  filesystem_safe = true;
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

  CONDITIONAL_SERIAL_PRINTLN("Reading configuration...");
  load_configuration();
  CONDITIONAL_SERIAL_PRINTLN("Configuration loaded.");
  config = get_config();

  if (configuration_loaded()) {
    delete[] ruuvi_devices;
    delete[] ruuvi_outdoor_sensor;
    delete[] ruuvi_readings;
    delete[] ruuvi_reading_time;
    ruuvi_devices        = new BD_ADDR[config.ruuvi.count];
    ruuvi_outdoor_sensor = new bool[config.ruuvi.count];
    ruuvi_readings       = new ruuvi_data_t[config.ruuvi.count];
    ruuvi_reading_time   = new time_t[config.ruuvi.count];

    for (uint8_t i = 0; i < config.ruuvi.count; i++) {
      ruuvi_data_t ruuvi_entry;
      ruuvi_devices[i] = BD_ADDR(config.ruuvi.devices[i].addr);
      ruuvi_outdoor_sensor[i] =
          (!strcmp("outdoor", config.ruuvi.devices[i].placement));
      ruuvi_readings[i]     = ruuvi_entry;
      ruuvi_reading_time[i] = 0;
    }
  }

  u8g2.setI2CAddress(I2C_ADDRESS << 1);
  u8g2.begin();
  u8g2.enableUTF8Print();
  setup_backlight();

  u8g2.clearBuffer();
  u8g2.drawXBM((u8g2.getDisplayWidth() >> 1) - (splash_logo_width >> 1),
               (u8g2.getDisplayHeight() >> 1) - (splash_logo_height >> 1),
               splash_logo_width, splash_logo_height, splash_logo_bits);
  u8g2.sendBuffer();

  // control_backlight(u8g2);
  delay(1000);

  u8g2.clearBuffer();
  u8g2.sendBuffer();

  CONDITIONAL_SERIAL_PRINTLN("Connecting to WiFi...");
  connect_network();
  CONDITIONAL_SERIAL_PRINTLN("WiFi connection configured.");
  pir_init();
}

/**
 * Main program loop.
 */
void loop() {
  delay(250);

  u8g2.setFont(u8g2_font_helvR08_tf);
  u8g2.clearBuffer();

  print_wifi_status(u8g2);
  if (configuration_loaded()) {
    print_time(u8g2, config);
  }
  if (configuration_loaded() && bluetooth_configured()) {
    print_bluetooth_status(u8g2);
    print_climate(u8g2, ruuvi_readings, ruuvi_outdoor_sensor,
                  config.ruuvi.count);
    process_pressure(ruuvi_readings, ruuvi_outdoor_sensor, config);
  }
  u8g2.sendBuffer();
  control_backlight(u8g2);

  if (configuration_loaded()) {
    control_wireless(config, advertisementCallback);
  }

  if (bluetooth_configured()) {
    control_bluetooth_scanning();
  }
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
    for (uint8_t i = 0; i < config.ruuvi.count; i++) {
      char known_addr[strlen(ruuvi_devices[i].getAddressString())];
      strcpy(known_addr, ruuvi_devices[i].getAddressString());
      if (!strcmp(adv_addr,
                  BD_ADDR(config.ruuvi.devices[i].addr).getAddressString())) {
        uint8_t data[LE_ADVERTISING_DATA_SIZE];
        memcpy(data, adv->getAdvData(), LE_ADVERTISING_DATA_SIZE);
        if (data[0] != 0x11) {
          time_t       now   = time(nullptr);
          ruuvi_data_t rdata = make_ruuvi_data(data);
          ruuvi_readings[i]  = rdata;
          if ((ruuvi_reading_time[i] == 0) ||
              difftime(now, ruuvi_reading_time[i]) >= 360.0f) {
            CONDITIONAL_SERIAL_PRINTLN(
                "Six minutes since last logged reading, saving...")
            ruuvi_reading_time[i] = now;
            CONDITIONAL_SERIAL_PRINT("Logging Ruuvi device: ");
            CONDITIONAL_SERIAL_PRINTLN(config.ruuvi.devices[i].name);
            CONDITIONAL_SERIAL_PRINT("Current pressure trend: ");
            CONDITIONAL_SERIAL_PRINTLN(pressure_trend());
            CONDITIONAL_SERIAL_PRINT("Zambretti trend: ");
            CONDITIONAL_SERIAL_PRINTLN(
                current_trend(pressure_trend()).baro_trend);
            CONDITIONAL_SERIAL_PRINT("Zambretti indication: ");
            CONDITIONAL_SERIAL_PRINTLN(
                current_trend(pressure_trend()).indication);
            if (average_pressure() > 0) {
              zambretti_forecast f = get_forecast(config);
              CONDITIONAL_SERIAL_PRINT("Forecast: ");
              CONDITIONAL_SERIAL_PRINT(f.forecast);
              CONDITIONAL_SERIAL_PRINT(": ");
              CONDITIONAL_SERIAL_PRINTLN(f.description);
            }
          }
        }
        // BTstack.bleConnect(adv, 10000);
      }
    }
  }
}
