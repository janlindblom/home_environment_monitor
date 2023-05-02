// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>

#include "forecast_types.h"

float pa_to_mb(uint32_t pressure_pa);
float pressure_to_slp(float pressure, int16_t elevation, float temperature);

char*              indication(int8_t baro_trend);
pressure_change    current_trend(float change);
zambretti_forecast get_forecast(float pressure, int8_t baro_trend);
