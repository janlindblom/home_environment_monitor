// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <U8g2lib.h>
#include <inttypes.h>

#include "configuration_types.h"
#include "ruuvi_types.h"

void print_climate(U8G2 u8g2, ruuvi_data_t ruuvi_readings[],
                   bool ruuvi_outdoor_sensor[], uint8_t ruuvi_sensor_count);
void process_pressure(ruuvi_data_t ruuvi_readings[],
                      bool ruuvi_outdoor_sensor[], Config configuration);

float   pressure_trend();
int32_t average_pressure();
float   average_temperature();
