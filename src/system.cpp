// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "system.h"

#include <Common.h>
#include <U8g2lib.h>
#include <inttypes.h>

#include "common.h"

uint8_t   _ambient_light  = 255;
uint16_t  adc_min         = 0; // Set this one to something else if you want.
uint16_t  adc_max         = 1023;
uint32_t  light_timer     = 0;
uint32_t  powersave_timer = 0;
bool      backlight_on    = true;
PinStatus _pir_state      = PinStatus::LOW;

/**
 * Reads a LDR connected between pins D22 and A0. Stores the reading as an
 * unsigned 8 bit integer.
 */
void check_ambient_light() {
  uint16_t total   = 0;
  uint16_t reading = 0;

  // Set D22 high then wait a little bit before reading A0.
  digitalWrite(D22, PinStatus::HIGH);
  delay(50);
  for (uint8_t i = 0; i < 4; i++) {
    total += analogRead(A0);
  }
  digitalWrite(D22, PinStatus::LOW);

  reading = total >> 2;

  // Reassign minimum value if what we read was lower.
  if (reading < adc_min) {
    adc_min = reading;
  }

  // Ambient light to use for backlighting. Remapped to 1-255.
  _ambient_light = map(reading, adc_min, adc_max, 1, 255);
}

/**
 * Controls the backlight level of the OLED screen. Reads the current ambient
 * light level and sets the display backlight accordingly.
 *
 * \param u8g2 the U8G2 display object.
 */
void control_backlight(U8G2 u8g2) {
  if (backlight_on) {
    u8g2.setPowerSave(0);
  } else if (!backlight_on && ((millis() - powersave_timer) >= 20000)) {
    u8g2.setPowerSave(1);
  }
  if ((millis() - light_timer) < 1000) {
    return;
  }
  check_ambient_light();
  light_timer = millis();
  u8g2.setContrast(_ambient_light);
}

/**
 * Configure the backlight control part.
 */
void setup_backlight() {
#if defined(ARDUINO_ARCH_RP2040)
  analogReadResolution(12);
  adc_max = 4095; // We get a bigger range on the rp2040
  pinMode(D22, PinMode::OUTPUT_12MA);
#else
  pinMode(D22, PinMode::OUTPUT);
#endif

  light_timer = millis();
}

void pir_triggered() {
  _pir_state = digitalRead(D21);
  if (_pir_state == PinStatus::HIGH || _pir_state == PinStatus::RISING) {
    // backlight_on = true;
    if (millis() - powersave_timer >= 1000) {
      powersave_timer = millis();
    }
  } else if (_pir_state == PinStatus::LOW || _pir_state == PinStatus::FALLING) {
    // backlight_on = false;
  }
}

PinStatus pir_state() {
  return _pir_state;
}

PinStatus pir_status() {
  _pir_state = digitalRead(D21);
  return _pir_state;
}

void pir_init() {
  pinMode(D21, PinMode::INPUT);
  attachInterrupt(digitalPinToInterrupt(D21), pir_triggered, PinStatus::CHANGE);
}

void pir_deinit() {
  detachInterrupt(digitalPinToInterrupt(D21));
}
