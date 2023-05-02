// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>

// Local timezone
#define TIMEZONE "Europe/Mariehamn"

#define FORCE_INLINE __attribute__((always_inline))

#define PIN_LDR A0
#define PIN_PIR D21
#define PIN_LDRPWR D22

// Coordinates for sunrise/sunset
#define LATITUDE 60.09726
#define LONGITUDE 19.93481
#define DST_OFFSET 3
#define ELEVATION 11

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
