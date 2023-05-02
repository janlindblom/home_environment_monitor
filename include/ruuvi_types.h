// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <inttypes.h>

typedef union ruuvi_device {
  char    name[16];
  char    placement[8];
  char    address[17];
  uint8_t addr[6];
} ruuvi_device_t;

typedef union ruuvi_data {
  uint8_t  format;
  float    temperature;
  float    humidity;
  uint32_t pressure;
} ruuvi_data_t;
