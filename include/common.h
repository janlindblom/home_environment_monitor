// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>

#include "font_segments_12x17.h"

#define FORCE_INLINE __attribute__((always_inline))

// Display parameters
#define I2C_ADDRESS 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Pin connections
#ifdef ARDUINO_ARCH_RP2040
#  define PIN_LDR A0
#  define PIN_LDR_PWR D22
#  define PIN_PIR D21
#endif
