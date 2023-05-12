// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "climate.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <U8g2lib.h>
#include <time.h>

#include <vector>

#include "common.h"
#include "configuration.h"
#include "configuration_types.h"
#include "forecast.h"
#include "ruuvi_types.h"

float    pressure_trend_data[2] = {0.0f, 0.0f};
int32_t  _pressure_trend        = 0;
time_t   last_pressure          = 0;
uint32_t _average_pressure      = 0;
float    _average_temperature   = 0;
float    pressure_readings[18];

float _average_temperatures[2] = {0.0f, 0.0f};

void log_pressure_reading(float new_last_element) {
  for (uint8_t i = 0; i < sizeof(pressure_readings) / sizeof(float) - 1; i++) {
    pressure_readings[i] = pressure_readings[i + 1];
  }
  pressure_readings[sizeof(pressure_readings) / sizeof(float) - 1] =
      new_last_element;
}

float get_pressure_trend() {
  return (pressure_readings[sizeof(pressure_readings) / sizeof(float) - 1] -
          pressure_readings[0]);
}

void process_pressure(ruuvi_data_t         ruuvi_readings[],
                      std::vector<uint8_t> ruuvi_outdoor_sensor) {
  if (!configured()) {
    return;
  }
  Config configuration = get_config();
  float  timediff      = difftime(time(nullptr), last_pressure);

  if ((timediff >= 10800.0f) ||
      ((timediff >= 60.0f) &&
       ((pressure_trend_data[0] == 0.0) || (pressure_trend_data[1] == 0.0)))) {
    uint8_t  number_of_readings = 0;
    uint32_t pressure_sum       = 0;
    uint32_t temperature_sum    = 0;
    for (uint8_t i = 0; i < configuration.ruuvi.count; i++) {
      if (ruuvi_outdoor_sensor[i]) {
        pressure_sum += ruuvi_readings[i].pressure;
        temperature_sum += ruuvi_readings[i].temperature;
        number_of_readings++;
      }
    }
    _average_pressure =
        pressure_sum / (number_of_readings >= 1 ? number_of_readings : 1);
    _average_temperature =
        temperature_sum /
        ((number_of_readings >= 1 ? number_of_readings : 1) * 1.0f);
    if (_average_pressure > 0) {
      pressure_trend_data[0] = pressure_trend_data[1];
      float slp              = pressure_to_slp(pa_to_mb(_average_pressure),
                                               configuration.location.elevation,
                                               _average_temperature);
      pressure_trend_data[1] = pa_to_mb(_average_pressure);

      Serial.print(F("Oldest pressure data: "));
      Serial.println(pressure_trend_data[0]);
      Serial.print(F("Newest pressure data: "));
      Serial.println(pressure_trend_data[1]);
      Serial.print(F("SLP: "));
      Serial.println(slp);
      last_pressure = time(nullptr);
    }
  }
}

int32_t average_pressure() {
  return _average_pressure;
}

float average_temperature() {
  return _average_temperature;
}

float pressure_trend() {
  return ((pressure_trend_data[0] == 0.0) || (pressure_trend_data[1] == 0.0)
              ? 0.0
              : pressure_trend_data[1] - pressure_trend_data[0]);
}

void log_current_temperature(float temperature, bool location_outdoor = false) {
  EEPROM.update(0 + (location_outdoor * sizeof(float)), temperature);
}

void print_climate(U8G2 u8g2, ruuvi_data_t ruuvi_readings[],
                   std::vector<uint8_t> ruuvi_outdoor_sensor) {
  uint8_t yoffset = 1;
  u8g2.setFont(u8g2_font_helvR08_tf);
  yoffset += u8g2.getMaxCharHeight();
  float   temperature_readings[] = {0.0, 0.0};
  float   humidity_readings[]    = {0.0, 0.0};
  uint8_t number_of_readings[]   = {0, 0};

  for (uint8_t i = 0; i < get_config().ruuvi.devices.size(); i++) {
    if (ruuvi_outdoor_sensor[i]) {
      temperature_readings[1] += ruuvi_readings[i].temperature;
      humidity_readings[1] += ruuvi_readings[i].humidity;
      number_of_readings[1]++;
    } else {
      temperature_readings[0] += ruuvi_readings[i].temperature;
      humidity_readings[0] += ruuvi_readings[i].humidity;
      number_of_readings[0]++;
    }
  }

  for (uint8_t i = 0; i < 2; i++) {
    u8g2.setFont(font_segments_12x17);
    yoffset += u8g2.getMaxCharHeight() + 2;
    char    temperature_string[8];
    char    humidity_string[6];
    uint8_t xoffset = 0;
    float   average_temperature =
        temperature_readings[i] /
        ((number_of_readings[i] > 0 ? number_of_readings[i] : 1) * 1.0f);
    float average_humidity =
        humidity_readings[i] /
        ((number_of_readings[i] > 0 ? number_of_readings[i] : 1) * 1.0f);
    sprintf(temperature_string, "%2.1f°C", average_temperature);
    sprintf(humidity_string, "%3d%c", int(average_humidity), '%');

    u8g2.setFont((i == 0 ? u8g2_font_open_iconic_embedded_2x_t
                         : u8g2_font_open_iconic_thing_2x_t));
    u8g2.drawGlyph(xoffset, yoffset, (i == 0 ? 68 : 76));
    xoffset += u8g2.getMaxCharWidth() + 2;
    u8g2.setFont(font_segments_12x17);
    u8g2.drawUTF8(xoffset, yoffset, temperature_string);

    u8g2.setFont(font_segments_12x17);
    xoffset = u8g2.getUTF8Width(humidity_string);
    u8g2.drawUTF8(u8g2.getDisplayWidth() - xoffset - 1, yoffset,
                  humidity_string);
  }
}
