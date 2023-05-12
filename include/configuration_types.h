// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <Arduino.h>

#include <string>
#include <vector>

#include "ruuvi_types.h"

typedef struct network_section_entry {
  std::string ssid;     // SSID of network
  std::string password; // Password for network
} network_section_entry_t;

typedef struct network_section {
  network_section_entry_t primary;   // Primary network
  network_section_entry_t secondary; // Secondary network
} network_section_t;

typedef struct location_section {
  float longitude; // Longitude of location
  float latitude;  // Latitude of location
  float tz_offset; // Timezone offset
  int   elevation; // Altitude above sea level
} location_t;

typedef struct ruuvi_section {
  uint8_t                     count;
  std::vector<ruuvi_device_t> devices; // Variable length list of sensors
} ruuvi_section_t;

struct Config {
  network_section_t networks; // Network section
  std::string       timezone; // Timezone
  location_t        location; // Geographic location section
  ruuvi_section_t   ruuvi;    // Ruuvi device section
};
