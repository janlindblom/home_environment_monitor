// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <inttypes.h>

#include "configuration_types.h"
#include "forecast_types.h"

float pa_to_mb(uint32_t pressure_pa);
float pressure_to_slp(float pressure, int16_t elevation, float temperature);

pressure_change_t    current_trend(float change);
zambretti_forecast_t get_forecast(Config configuration);
