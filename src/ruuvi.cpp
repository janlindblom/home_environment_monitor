// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ruuvi.h"

#include <Arduino.h>
#include <BTstackLib.h>
#include <time.h>

#include <string>
#include <vector>

#include "common.h"
#include "configuration.h"
#include "ruuvi_types.h"

std::vector<BD_ADDR>      _ruuvi_devices;
std::vector<uint8_t>      _ruuvi_outdoor_sensor;
std::vector<ruuvi_data_t> _ruuvi_readings;
std::vector<time_t>       _ruuvi_reading_time;

bool _ruuvi_devices_configured = false;

void setup_ruuvi_devices() {
  Config configuration      = get_config();
  _ruuvi_devices_configured = false;
  if (configured()) {
    size_t devices = configuration.ruuvi.devices.size();

    for (size_t i = 0; i < devices; i++) {
      ruuvi_data_t ruuvi_entry;
      _ruuvi_devices.push_back(configuration.ruuvi.devices[i].addr);
      _ruuvi_outdoor_sensor.push_back(
          !strcmp("outdoor", configuration.ruuvi.devices[i].placement.c_str()));
      _ruuvi_readings.push_back(ruuvi_entry);
      _ruuvi_reading_time.push_back(time(nullptr));
    }
    _ruuvi_devices.shrink_to_fit();
    _ruuvi_reading_time.shrink_to_fit();
    _ruuvi_outdoor_sensor.shrink_to_fit();
    _ruuvi_readings.shrink_to_fit();
    _ruuvi_devices_configured = true;
  }
}

/**
 * Transform data from the BLE Advertisement package into a struct we can use.
 *
 * @see
 * <https://docs.ruuvi.com/communication/bluetooth-advertisements/data-format-5-rawv2>
 */
ruuvi_data_t make_ruuvi_data(uint8_t data[]) {
  // This is where the data starts in the BLE advertisement package.
  uint8_t      offset = 7;
  ruuvi_data_t res;
  uint16_t pres   = makeWord(data[offset + 5], data[offset + 6]); // offset 5-6
  uint16_t hum    = makeWord(data[offset + 3], data[offset + 4]); // offset 3-4
  int16_t  temp   = makeWord(data[offset + 1], data[offset + 2]); // offset 1-2
  res.format      = data[offset];                                 // offset 0
  res.temperature = temp * 0.005f;
  res.humidity    = hum * 0.0025f;
  res.pressure    = pres + 50000;
  return res;
}

bool ruuvi_devices_configured() {
  return _ruuvi_devices_configured;
}

std::vector<uint8_t> ruuvi_outdoor_sensor() {
  return _ruuvi_outdoor_sensor;
}

std::vector<ruuvi_data_t> ruuvi_readings() {
  return _ruuvi_readings;
}

void store_ruuvi_reading(uint8_t i, ruuvi_data_t rdata) {
  _ruuvi_readings[i] = rdata;
}

std::vector<time_t> ruuvi_reading_times() {
  return _ruuvi_reading_time;
}

void store_ruuvi_reading_time(size_t i, time_t time) {
  _ruuvi_reading_time[i] = time;
}
