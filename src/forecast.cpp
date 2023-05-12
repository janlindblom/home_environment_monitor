// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "forecast.h"

#include <Arduino.h>
#include <U8g2lib.h>
#include <inttypes.h>

#include "climate.h"
#include "common.h"
#include "configuration.h"
#include "forecast_types.h"
#include "network_time.h"

const pressure_change_t change_slp[9] PROGMEM = {
    {6.0f, "Rising Very Rapidly", 4},
    {3.6f, "Rising Quickly", 3},
    {1.6f, "Rising", 2},
    {0.1f, "Rising Slowly", 1},
    {-0.1f, "Steady", 0},
    {-1.6f, "Falling Slowly", -1},
    {-3.6f, "Falling", -2},
    {-6.0f, "Falling Quickly", -3},
    {-6.0f, "Falling Very Rapidly", -4}};

char* forecast_strings[4] PROGMEM = {"Settled Fine", "Fine Weather",
                                     "Very Unsettled, Rain",
                                     "Stormy, much rain"};

const zambretti_forecast_t forecast[32] PROGMEM = {
    {'A', forecast_strings[0]},
    {'B', forecast_strings[1]},
    {'D', "Fine Becoming Less Settled"},
    {'H', "Fairly Fine Showery Later"},
    {'O', "Showery Becoming more unsettled"},
    {'R', "Unsettled, Rain later"},
    {'U', "Rain at times, worse later"},
    {'V', "Rain at times, becoming very unsettled"},
    {'X', forecast_strings[2]},

    {'A', forecast_strings[0]},
    {'B', forecast_strings[1]},
    {'E', "Fine, Possibly showers"},
    {'K', "Fairly Fine, Showers likely"},
    {'N', "Showery Bright Intervals"},
    {'P', "Changeable some rain"},
    {'S', "Unsettled, rain at times"},
    {'W', "Rain at Frequent Intervals"},
    {'X', forecast_strings[2]},
    {'Z', forecast_strings[3]},

    {'A', forecast_strings[0]},
    {'B', forecast_strings[1]},
    {'C', "Becoming Fine"},
    {'F', "Fairly Fine, Improving"},
    {'G', "Fairly Fine, Possibly showers, early"},
    {'I', "Showery Early, Improving"},
    {'J', "Changeable Mending"},
    {'L', "Rather Unsettled Clearing Later"},
    {'M', "Unsettled, Probably Improving"},
    {'Q', "Unsettled, short fine Intervals"},
    {'T', "Very Unsettled, Finer at times"},
    {'Y', "Stormy, possibly improving"},
    {'Z', forecast_strings[3]}};

enum icon_name {
  CLOUDY = 0,
  STORMY,
  STORMY_RAINY,
  RAINY_SNOWY,
  RAINY,
  SHOWERY,
  OVERCAST_SUN,
  OVERCAST_MOON,
  CLEAR_MOON,
  CLEAR_SUN,
  WINDY
};

/**
 * Translate pressure in Pascals to pressure in mBar.
 *
 * \param pressure_pa pressure in Pascals
 * \return the current pressure in mBar
 */
float pa_to_mb(uint32_t pressure_pa) {
  return 0.01f * pressure_pa;
}

/**
 * Translate local pressure in mBar to pressure at sealevel in mBar.
 *
 * \param pressure local pressure in mBar
 * \param elevation elevation from sealevel in meters
 * \param temperature current temperature in Celsius
 * \return the current pressure at sealevel expressed in mBar
 */
float pressure_to_slp(float pressure, int16_t elevation, float temperature) {
  // Use single precision floating point since double precision is slower.
  return pressure * pow(1 - 0.0065f * elevation /
                                (temperature + 0.0065f * elevation + 273.15f),
                        -5.275f);
}

/**
 * Get the current Zambretti barometric trend from the current trend of change
 * in pressure.
 *
 * \param change the current pressure change trend in mBar/time
 */
pressure_change_t current_trend(float change) {
  if (change > change_slp[0].threshold) {
    return change_slp[0];
  } else if (change > change_slp[1].threshold) {
    return change_slp[1];
  } else if (change > change_slp[2].threshold) {
    return change_slp[2];
  } else if (change > change_slp[3].threshold) {
    return change_slp[3];
  } else if (change > change_slp[4].threshold) {
    return change_slp[4];
  } else if (change > change_slp[5].threshold) {
    return change_slp[5];
  } else if (change > change_slp[6].threshold) {
    return change_slp[6];
  } else if (change > change_slp[7].threshold) {
    return change_slp[7];
  } else if (change <= change_slp[8].threshold) {
    return change_slp[8];
  }

  // Return "Steady" as a fallback, should not reach this point.
  return change_slp[4];
}

/**
 * Tell if it's currently summer.
 */
bool summer() {
  time_t    now = time(nullptr);
  struct tm local;
  localtime_r(&now, &local);
  // Summer is may through october
  if (local.tm_mon >= 4 && local.tm_mon <= 9) {
    return true;
  }
  return false;
}

/**
 * Tell if it's currently daytime.
 */
bool day() {
  time_t    now = time(nullptr);
  struct tm local, gmt;
  localtime_r(&now, &local);
  gmtime_r(&now, &gmt);
  sun().setCurrentDate(gmt.tm_year + 1900, gmt.tm_mon + 1, gmt.tm_mday);
  int sunrise = static_cast<int>(sun().calcSunrise());
  int sunset  = static_cast<int>(sun().calcSunset());
  return ((local.tm_hour * 60 + local.tm_min >= sunrise) &&
          (local.tm_hour * 60 + local.tm_min < sunset));
}

/**
 * Get a forecast based on the Zambretti algorithm.
 *
 * \param configuration the parsed config file
 */
zambretti_forecast_t get_forecast() {
  Config configuration = get_config();
  int    z             = 1;
  int    trend         = current_trend(pressure_trend()).baro_trend;
  int    pressure      = int(pressure_to_slp(pa_to_mb(average_pressure()),
                                             configuration.location.elevation,
                                             average_temperature()));

  if (trend > 0) {
    // For a rising barometer Z = 179-P*0.16
    z = int(179 - (20 * pressure) / 129);
    z -= summer(); // Subtract one if it's summer
  } else if (trend < 0) {
    // For a falling barometer Z = 130-P*0.12
    z = int(130 - (10 * pressure) / 81);
    z -= !summer(); // Subtract one if it's winter
  } else {
    // For a steady barometer Z = 147-P*0.13
    z = int(147 - (50 * pressure) / 376);
  }

  // Make sure we're not out of bounds.
  if (z >= sizeof(forecast) / sizeof(zambretti_forecast_t)) {
    z -= 1;
  } else if (z < 0) {
    z += 1;
  }

  // Since calculated Z will be >= 1, subtract 1 to use it as array index.
  return forecast[z - 1];
}

/**
 * Print an icon representing the current forecast on the OLED. Displays as a
 * little icon between the wireless indicators and the sunrise/sunset times.
 *
 * \param u8g2 the OLED display
 * \param configuration the processed config file
 */
void print_forecast_icon(U8G2 u8g2) {
  zambretti_forecast_t forecast = get_forecast();

  u8g2.setFont(u8g2_font_siji_t_6x10);
  u8g2.drawGlyph(u8g2.getDisplayWidth() - (2 * u8g2.getMaxCharWidth()) - 1,
                 u8g2.getDisplayHeight() - 1,
                 forecast_icon(forecast.forecast, day()));
}

/**
 * Find a fitting icon representing the given Zambretti forecast code.
 *
 * \param forecast the Zambretti forecast code
 * \param day whether it's daytime or not
 */
uint16_t forecast_icon(char forecast, bool day = true) {
  uint16_t icon_offset = 57899;
  uint16_t icon        = 63;
  switch (forecast) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      icon = icon_offset + (day ? CLEAR_SUN : CLEAR_MOON);
      break;
    case 'E':
    case 'F':
    case 'J':
      icon = icon_offset + (day ? OVERCAST_SUN : OVERCAST_MOON);
      break;
    case 'G':
    case 'H':
    case 'L':
    case 'M':
    case 'Q':
      icon = icon_offset + CLOUDY;
      break;
    case 'I':
    case 'K':
    case 'N':
    case 'O':
    case 'P':
    case 'R':
    case 'S':
      icon = icon_offset + SHOWERY;
      break;
    case 'T':
    case 'U':
    case 'W':
      icon = icon_offset + RAINY;
      break;
    case 'V':
    case 'X':
      icon = icon_offset + STORMY_RAINY;
      break;
    case 'Y':
    case 'Z':
      icon = icon_offset + STORMY;
      break;
  }

  return icon;
}
