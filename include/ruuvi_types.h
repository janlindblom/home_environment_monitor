// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <Arduino.h>
#include <BTstackLib.h>

#include <string>
typedef struct ruuvi_device {
  std::string name;      // Variable length device name string
  std::string placement; // Variable length device placement string
  char        address[17];
  uint8_t     addr[6];
  BD_ADDR     bt_addr;
} ruuvi_device_t;

typedef struct ruuvi_data {
  uint8_t  format      = 5;
  float    temperature = 0.0;
  float    humidity    = 0.0;
  uint32_t pressure    = 0;
} ruuvi_data_t;
