// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <U8g2lib.h>
#include <sunset.h>

#include "configuration_types.h"

#ifndef NTP_SERVER1
#  define NTP_SERVER1 "0.fi.pool.ntp.org"
#endif
#ifndef NTP_SERVER2
#  define NTP_SERVER2 "1.fi.pool.ntp.org"
#endif
#define NTP_TIMEOUT 3600

void   configure_network_time();
bool   network_time_set();
bool   network_time_received();
void   print_time();
void   configure_sunset();
SunSet sun();
