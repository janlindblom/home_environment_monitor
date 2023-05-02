// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <Arduino.h>

/**
 * Data structure describing a pressure change.
 */
typedef struct pressure_change {
  float  threshold;  // Threshold for level
  char*  indication; // Description of level
  int8_t baro_trend; // Zambretti barometric trend
} pressure_change_t;

/**
 * Data structure describing a forecast based on the Zambretti forecaster.
 */
typedef struct zambretti_forecast {
  char  forecast;    // Zambretti forecast letter
  char* description; // Description for forecast letter
} zambretti_forecast_t;
