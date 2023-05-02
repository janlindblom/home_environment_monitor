// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "forecast.h"

#include <Arduino.h>

#include "common.h"
#include "forecast_types.h"

const int8_t falling_index = -1;
const int8_t rising_index  = 1;

const pressure_change change_slp[9] = {
    {6.0f, "Rising Very Rapidly", 4, true},
    {3.6f, "Rising Quickly", 3, true},
    {1.6f, "Rising", 2, true},
    {0.1f, "Rising Slowly", 1, true},
    {-0.1f, "Steady", 0, true},
    {-1.6f, "Falling Slowly", -1, true},
    {-3.6f, "Falling", -2, true},
    {-6.0f, "Falling Quickly", -3, true},
    {-6.0f, "Falling Very Rapidly", -4, false}};

const char* forecast_strings[4] = {"Settled Fine", "Fine Weather",
                                   "Very Unsettled, Rain", "Stormy, much rain"};

const zambretti_forecast forecast[] = {
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
 * Get the Zambretti text indication for the current barometric trend.
 *
 * \param baro_trend the current Zambretti barometric trend
 * \return a string describing the current trend
 */
char* indication(int8_t baro_trend) {
  for (uint8_t i = 0; i < 9; i++) {
    if (baro_trend == change_slp[i].baro_trend) {
      return change_slp[i].indication;
    }
  }

  return change_slp[4].indication;
}

/**
 * Get the current Zambretti barometric trend from the current trend of change
 * in pressure.
 *
 * \param change the current pressure change trend in mBar/time
 */
pressure_change current_trend(float change) {
  if (change > 6.0f) {
    return change_slp[0];
  } else if (change > 3.6f) {
    return change_slp[1];
  } else if (change > 1.6f) {
    return change_slp[2];
  } else if (change > 0.1f) {
    return change_slp[3];
  } else if (change > -0.1f) {
    return change_slp[4];
  } else if (change > -1.6f) {
    return change_slp[5];
  } else if (change > -3.6f) {
    return change_slp[6];
  } else if (change > -6.0f) {
    return change_slp[7];
  } else if (change <= -6.0f) {
    return change_slp[8];
  }

  // Return "Steady" as a fallback, should not reach this point.
  return change_slp[4];
}

zambretti_forecast get_forecast(float pressure, int8_t baro_trend) {
  int z          = 1;
  int pressure_i = int(pressure);
  CONDITIONAL_SERIAL_PRINT("Baro trend: ");
  CONDITIONAL_SERIAL_PRINTLN(baro_trend);
  CONDITIONAL_SERIAL_PRINT("Pressure: ");
  CONDITIONAL_SERIAL_PRINT(pressure);
  CONDITIONAL_SERIAL_PRINT(", ");
  CONDITIONAL_SERIAL_PRINTLN(pressure_i);

  if (baro_trend > 0) {
    // For a rising barometer Z = 179-2P/129
    z = int(179 - pressure * 0.16f);

  } else if (baro_trend < 0) {
    // For a falling barometer Z = 130-P/81
    z = int(130 - pressure * 0.12f);
  } else {
    // For a steady barometer Z = 147-5P/376
    z = int(147 - pressure * 0.13f);
  }
  CONDITIONAL_SERIAL_PRINT("Z: ");
  CONDITIONAL_SERIAL_PRINTLN(z);

  // Since calculated Z will be >= 1, subtract 1 to use it as array index.
  return forecast[z - 1];
}
