// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "system.h"

#include <common.h>
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
 * Reads a LDR connected between pins PIN_LDR_PWR and PIN_LDR. Stores the
 * reading as an unsigned 8 bit integer.
 */
void check_ambient_light() {
  uint16_t total   = 0;
  uint16_t reading = 0;

// Set PIN_LDR_PWR high then wait a little bit before reading PIN_LDR.
#ifdef __AVR__
  digitalWrite(PIN_LDR_PWR,
               PinStatus::HIGH); // TODO: Replace with direct PORT manipulation
#else
  digitalWrite(PIN_LDR_PWR, PinStatus::HIGH);
#endif
  delay(50);
  for (uint8_t i = 0; i < 4; i++) {
    total += analogRead(PIN_LDR);
  }
#ifdef __AVR__
  digitalWrite(PIN_LDR_PWR,
               PinStatus::LOW); // TODO: Replace with direct PORT manipulation
#else
  digitalWrite(PIN_LDR_PWR, PinStatus::LOW);
#endif

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
 */
void control_backlight() {
  if ((millis() - light_timer) < 1000) {
    return;
  }
  U8G2 u8g2 = get_display();
  if (backlight_on) {
    u8g2.setPowerSave(0);
  } else if (!backlight_on && ((millis() - powersave_timer) >= 20000)) {
    u8g2.setPowerSave(1);
  }
  check_ambient_light();
  light_timer = millis();
  u8g2.setContrast(_ambient_light);
}

/**
 * Configure the backlight control part.
 */
void setup_backlight() {
#ifndef __AVR__
  analogReadResolution(12);
  adc_max = 4095; // We get a bigger range on the rp2040
  pinMode(PIN_LDR_PWR, PinMode::OUTPUT_12MA);
#else
  pinMode(PIN_LDR_PWR, PinMode::OUTPUT);
#endif

  light_timer = millis();
}

void pir_triggered() {
  _pir_state = digitalRead(PIN_PIR);
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
  _pir_state = digitalRead(PIN_PIR);
  return _pir_state;
}

void pir_init() {
  pinMode(PIN_PIR, PinMode::INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_PIR), pir_triggered,
                  PinStatus::CHANGE);
}

void pir_deinit() {
  detachInterrupt(digitalPinToInterrupt(PIN_PIR));
}
