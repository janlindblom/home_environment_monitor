// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <Arduino.h>

struct pressure_change {
  float  threshold;  // Threshold for trend
  char*  indication; // Description of level
  int8_t baro_trend; // Zambretti baroTrend
  bool   gt;         // Use > for comparison, otherwise <=.
};

struct zambretti_forecast {
  char        forecast;
  const char* description;
};
