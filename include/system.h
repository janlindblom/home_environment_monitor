// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <U8g2lib.h>

void check_ambient_light();
void control_backlight(U8G2 u8g2);
void setup_backlight();

PinStatus pir_state();
void      pir_init();
void      pir_deinit();
PinStatus pir_status();
