// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <U8g2lib.h>

#include "font_segments_12x17.h"

#define FORCE_INLINE __attribute__((always_inline))

// Display parameters
#define I2C_ADDRESS 0x3c

// Pin connections
#ifdef __AVR__
#  define PIN_LDR A0
#  define PIN_LDR_PWR D13
#  define PIN_PIR D12
#else
#  define PIN_LDR A0
#  define PIN_LDR_PWR D22
#  define PIN_PIR D21
#endif

bool is_filesystem_safe();
U8G2 get_display();
