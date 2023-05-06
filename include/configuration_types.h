// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <Arduino.h>

#include "ruuvi_types.h"

typedef struct network_section_entry {
  char* ssid;
  char* password;
} network_section_entry_t;

typedef struct network_section {
  network_section_entry_t primary;
  network_section_entry_t secondary;
} network_section_t;

typedef struct location_section {
  float longitude;
  float latitude;
  float tz_offset;
  int   elevation;
} location_t;

typedef struct ruuvi_section {
  uint8_t        count;
  ruuvi_device_t devices[10];
} ruuvi_t;

struct Config {
  network_section_t networks;
  char              timezone[64];
  location_t        location;
  ruuvi_t           ruuvi;
};
