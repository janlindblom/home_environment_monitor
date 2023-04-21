// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiNTP.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>

#include "arduino_secrets.h"

#ifndef NTP_SERVER1
#  define NTP_SERVER1 "0.fi.pool.ntp.org"
#endif
#ifndef NTP_SERVER2
#  define NTP_SERVER2 "1.fi.pool.ntp.org"
#endif
#define NTP_TIMEOUT 10000

#define I2C_ADDRESS 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16

#ifndef STASSID
#  define STASSID "no-network"
#  define STAPSK "no-password"
#endif

#define CONDITIONAL_SERIAL_PRINTLN(str) \
  {                                     \
    if (Serial) {                       \
      Serial.println(str);              \
    }                                   \
  }

#define CONDITIONAL_SERIAL_PRINT(str) \
  {                                   \
    if (Serial) {                     \
      Serial.print(str);              \
    }                                 \
  }

void print_time();
