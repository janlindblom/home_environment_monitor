// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <BTstackLib.h>
#include <inttypes.h>
#include <time.h>

#include "configuration_types.h"
#include "ruuvi_types.h"

void setup_ruuvi_devices(Config configuration);

bool          ruuvi_devices_configured();
BD_ADDR*      ruuvi_devices();
bool*         ruuvi_outdoor_sensor();
ruuvi_data_t* ruuvi_readings();
time_t*       ruuvi_reading_times();

ruuvi_data_t make_ruuvi_data(uint8_t data[]);
void         store_ruuvi_reading(uint8_t i, volatile ruuvi_data_t rdata);
void         store_ruuvi_reading_time(uint8_t i, volatile time_t time);
