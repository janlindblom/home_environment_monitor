// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "climate.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <time.h>

#include "common.h"
#include "configuration_types.h"
#include "forecast.h"
#include "ruuvi.h"
#include "ruuvi_types.h"

float    pressure_trend_data[2] = {0.0f, 0.0f};
int32_t  _pressure_trend        = 0;
time_t   last_pressure          = 0;
uint32_t _average_pressure      = 0;
float    _average_temperature   = 0;

void load_climate_log(Stream& input) {
  StaticJsonDocument<192> doc;
  DeserializationError    error = deserializeJson(doc, input);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  for (JsonPair pressure_item : doc["pressure"].as<JsonObject>()) {
    const char* pressure_item_key =
        pressure_item.key().c_str(); // "oldest", "newest"

    time_t   pressure_item_value_time  = pressure_item.value()["time"];
    uint32_t pressure_item_value_value = pressure_item.value()["value"];
  }
}

void store_climate_log(Stream& output) {
  StaticJsonDocument<128> doc;

  JsonObject pressure = doc.createNestedObject("pressure");

  JsonObject pressure_oldest = pressure.createNestedObject("oldest");
  pressure_oldest["time"]    = 0;
  pressure_oldest["value"]   = 0;

  JsonObject pressure_newest = pressure.createNestedObject("newest");
  pressure_newest["time"]    = 0;
  pressure_newest["value"]   = 0;

  serializeJson(doc, output);
}

void process_pressure(ruuvi_data_t ruuvi_readings[],
                      bool ruuvi_outdoor_sensor[], uint8_t ruuvi_sensor_count,
                      Config configuration) {
  float timediff = difftime(time(nullptr), last_pressure);

  if ((timediff >= 10800.0f) ||
      ((timediff >= 60.0f) &&
       ((pressure_trend_data[0] == 0.0) || (pressure_trend_data[1] == 0.0)))) {
    uint8_t  number_of_readings = 0;
    uint32_t pressure_sum       = 0;
    uint32_t temperature_sum    = 0;
    for (uint8_t i = 0; i < ruuvi_sensor_count; i++) {
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
      CONDITIONAL_SERIAL_PRINT("Oldest pressure data: ");
      CONDITIONAL_SERIAL_PRINTLN(pressure_trend_data[0]);
      CONDITIONAL_SERIAL_PRINT("Newest pressure data: ");
      CONDITIONAL_SERIAL_PRINTLN(pressure_trend_data[1]);
      CONDITIONAL_SERIAL_PRINT("SLP: ");
      CONDITIONAL_SERIAL_PRINTLN(slp);
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

void print_climate(U8G2 u8g2, ruuvi_data_t ruuvi_readings[],
                   bool ruuvi_outdoor_sensor[], uint8_t ruuvi_sensor_count) {
  uint8_t yoffset = 1;
  u8g2.setFont(u8g2_font_helvR08_tf);
  yoffset += u8g2.getMaxCharHeight();
  float   temperature_readings[] = {0.0, 0.0};
  float   humidity_readings[]    = {0.0, 0.0};
  uint8_t number_of_readings[]   = {0, 0};

  for (uint8_t i = 0; i < ruuvi_sensor_count; i++) {
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
    u8g2.setFont(u8g2_font_helvB10_tf);
    yoffset += u8g2.getMaxCharHeight();
    char    temperature_string[9];
    char    humidity_string[7];
    uint8_t xoffset = 0;
    float   average_temperature =
        temperature_readings[i] /
        ((number_of_readings[i] >= 1 ? number_of_readings[i] : 1) * 1.0f);
    float average_humidity =
        humidity_readings[i] /
        ((number_of_readings[i] >= 1 ? number_of_readings[i] : 1) * 1.0f);
    sprintf(temperature_string, "%2.1f°C", average_temperature);
    sprintf(humidity_string, "%3.1f%c", average_humidity, '%');

    u8g2.setFont(u8g2_font_unifont_t_weather);
    u8g2.drawGlyph(xoffset, yoffset, 49);
    xoffset += u8g2.getMaxCharWidth();
    u8g2.setFont(u8g2_font_helvB10_tf);
    u8g2.drawUTF8(xoffset, yoffset, temperature_string);

    xoffset += u8g2.getUTF8Width(temperature_string);
    u8g2.setFont(u8g2_font_unifont_t_weather);
    u8g2.drawGlyph(xoffset, yoffset, 50);
    xoffset += u8g2.getMaxCharWidth();
    u8g2.setFont(u8g2_font_helvR10_tf);
    u8g2.drawStr(xoffset, yoffset, humidity_string);
    xoffset += u8g2.getStrWidth(humidity_string);
  }
}
