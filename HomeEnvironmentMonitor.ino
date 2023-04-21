// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiNTP.h>
#include <WiFiUdp.h>
#include <btstack.h>
#include <sys/time.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "arduino_secrets.h"

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SH110X display = Adafruit_SH110X(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
// clang-format off
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ 0, 192,
  1, 192,
  1, 192,
  3, 224,
  243, 224,
  254, 248,
  126, 254,
  51, 159,
  31, 252,
  13, 112,
  27, 160,
  63, 224,
  63, 240,
  124, 240,
  112, 112,
  0, 48
};
// clang-format on

void setup() {
    delay(250);                       // wait for the OLED to power up
    display.begin(i2c_Address, true); // Address 0x3C default
    display.display();
    delay(2000);

    // Clear the buffer.
    display.clearDisplay();

    // miniature bitmap display
    display.drawBitmap(30, 16, logo16_glcd_bmp, 16, 16, 1);
    display.display();
}

void loop() {
    // TBD
}
