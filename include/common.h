// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>

#define FORCE_INLINE __attribute__((always_inline))

// Display parameters
#define I2C_ADDRESS 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define CONDITIONAL_SERIAL_PRINTLN(...) \
  {                                     \
    if (Serial) {                       \
      Serial.println(__VA_ARGS__);      \
    }                                   \
  }

#define CONDITIONAL_SERIAL_PRINT(...) \
  {                                   \
    if (Serial) {                     \
      Serial.print(__VA_ARGS__);      \
    }                                 \
  }
