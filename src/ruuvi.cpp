// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ruuvi.h"

#include <Arduino.h>

// #include <list>

#include "common.h"
#include "ruuvi_types.h"

ruuvi_device_t* ruuvi_device_data = 0;
ruuvi_data_t*   ruuvi_data        = 0;
// std::list<ruuvi_data_t> ruuvi_log;

void setup_ruuvi_storage(unsigned int size) {
  if (ruuvi_device_data != 0) {
    delete[] ruuvi_device_data;
  }
  ruuvi_device_data = new ruuvi_device_t[size];
}

void setup_ruuvi_data_storage(unsigned int size) {
  if (ruuvi_data != 0) {
    delete[] ruuvi_data;
  }
  ruuvi_data = new ruuvi_data_t[size];
}

/**
 * Transform data from the BLE Advertisement package into a struct we can use.
 *
 * \see
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
