// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "network_time.h"

#include <U8g2lib.h>
#include <WiFi.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <timezonedb_lookup.h>

#include "common.h"
#include "configuration.h"
#include "configuration_types.h"
#include "wireless.h"

const char* weekdays[] = {"Söndag",  "Måndag", "Tisdag", "Onsdag",
                          "Torsdag", "Fredag", "Lördag"};
const char* months[]   = {"jan", "feb", "mar", "apr", "maj", "jun",
                          "jul", "aug", "sep", "okt", "nov", "dec"};

bool _network_time_set      = false;
bool _network_time_received = false;

SunSet _sun;

void configure_network_time(Config configuration) {
  if (!network_connected()) {
    return;
  }

  if (!NTP.running()) {
    CONDITIONAL_SERIAL_PRINTLN("Setting up NTP...");
    NTP.begin(NTP_SERVER1, NTP_SERVER2);
  } else {
    CONDITIONAL_SERIAL_PRINTLN(
        "NTP configured. Waiting for time from network.");
  }

  if (NTP.waitSet()) {
    CONDITIONAL_SERIAL_PRINTLN("Processing NTP response...");
    _network_time_received = true;
  } else {
    CONDITIONAL_SERIAL_PRINTLN(
        "Timeout waiting for network time, will wait for response.");
    _network_time_received = false;
  }

  time_t now = time(nullptr);
  if (now > 57600) {
    CONDITIONAL_SERIAL_PRINTLN("NTP time response from network, processing.");
    const char* tz = lookup_posix_timezone_tz(configuration.timezone);
    setenv("TZ", tz, 1);
    tzset();

    CONDITIONAL_SERIAL_PRINT("Time set from network: ");
    CONDITIONAL_SERIAL_PRINT(asctime(localtime(&now)));
    configure_sunset(configuration);
    _network_time_set = true;
  } else {
    CONDITIONAL_SERIAL_PRINTLN(
        "No good time from network yet, will try again.");
    _network_time_set = false;
  }
}

void print_time(U8G2 u8g2, Config configuration) {
  if (network_time_set()) {
    time_t now = time(nullptr);

    if (now > 57600) {
      struct tm local;
      char      sunrise_string[6];
      char      sunset_string[5];
      char      time_string[5];
      uint8_t   sunrise_str_offset = 0;

      int sunrise = static_cast<int>(_sun.calcSunrise());
      int sunset  = static_cast<int>(_sun.calcSunset());

      localtime_r(&now, &local);
      char date_string[strlen(weekdays[local.tm_wday]) +
                       strlen(months[local.tm_mon]) +
                       (local.tm_mday > 9 ? 2 : 1) + 5];

      u8g2.setFont(u8g2_font_helvR08_tf);
      sprintf(date_string, "%s %d %s %4d", weekdays[local.tm_wday],
              local.tm_mday, months[local.tm_mon], (local.tm_year + 1900));
      u8g2.drawUTF8(0, u8g2.getMaxCharHeight(), date_string);
      u8g2.setFont(u8g2_font_helvB08_tf);
      sprintf(time_string, "%02d:%02d", local.tm_hour, local.tm_min);
      u8g2.drawStr((u8g2.getDisplayWidth() - u8g2.getStrWidth(time_string)),
                   u8g2.getMaxCharHeight(), time_string);

      u8g2.setFont(u8g2_font_open_iconic_weather_1x_t);
      u8g2.drawGlyph(0, u8g2.getDisplayHeight() - 1, 0x45);
      sunrise_str_offset += u8g2.getMaxCharWidth();
      u8g2.setFont(u8g2_font_helvR08_tf);
      sprintf(sunrise_string, "%02d:%02d ", (sunrise / 60), (sunrise % 60));
      u8g2.drawStr(sunrise_str_offset, u8g2.getDisplayHeight() - 1,
                   sunrise_string);
      sunrise_str_offset += u8g2.getStrWidth(sunrise_string);
      u8g2.setFont(u8g2_font_open_iconic_weather_1x_t);
      u8g2.drawGlyph(sunrise_str_offset, u8g2.getDisplayHeight() - 1, 0x42);
      sunrise_str_offset += u8g2.getMaxCharWidth();
      u8g2.setFont(u8g2_font_helvR08_tf);
      sprintf(sunset_string, "%02d:%02d", (sunset / 60), (sunset % 60));
      u8g2.drawStr(sunrise_str_offset, u8g2.getDisplayHeight() - 1,
                   sunset_string);
    }
  }
}

void configure_sunset(Config configuration) {
  CONDITIONAL_SERIAL_PRINTLN("Setting up SunSet...");
  time_t    now = time(nullptr);
  struct tm gmt;
  gmtime_r(&now, &gmt);
  _sun.setCurrentDate(gmt.tm_year + 1900, gmt.tm_mon + 1, gmt.tm_mday);
  _sun.setPosition(configuration.location.latitude,
                   configuration.location.longitude,
                   configuration.location.tz_offset);
}

SunSet sun() {
  return _sun;
}

bool network_time_set() {
  return _network_time_set;
}

bool network_time_received() {
  return _network_time_received;
}

void set_network_time_set(bool val) {
  _network_time_set = val;
}

void set_network_time_received(bool val) {
  _network_time_received = val;
}
