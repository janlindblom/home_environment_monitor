// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <inttypes.h>

#include "ruuvi_types.h"

typedef union network_section_entry {
  char ssid[32];
  char password[63];
} network_section_entry_t;

typedef union network_section {
  network_section_entry_t primary;
  network_section_entry_t secondary;
} network_section_t;

typedef union location_section {
  float longitude;
  float latitude;
  float tz_offset;
  int   elevation;
} location_t;

typedef union ruuvi_section {
  uint8_t        count;
  ruuvi_device_t devices[10];
} ruuvi_t;

typedef struct config_structure {
  network_section_t networks;
  char              timezone[64];
  location_t        location;
  ruuvi_t           ruuvi;
} Config;
