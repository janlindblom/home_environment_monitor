// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <U8g2lib.h>

#include <vector>

#include "configuration_types.h"
#include "ruuvi_types.h"

void print_climate();
void process_pressure();

void log_current_temperature(float temperature, bool location_outdoor);

float   pressure_trend();
int32_t average_pressure();
float   average_temperature();
