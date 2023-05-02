// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <U8g2lib.h>
#include <inttypes.h>
#include <sunset.h>

#include "configuration_types.h"

#ifndef NTP_SERVER1
#  define NTP_SERVER1 "0.fi.pool.ntp.org"
#endif
#ifndef NTP_SERVER2
#  define NTP_SERVER2 "1.fi.pool.ntp.org"
#endif
#define NTP_TIMEOUT 3600

void configure_network_time(Config config);
bool network_time_set();
bool network_time_received();
void print_time(U8G2 u8g2, Config config);
void configure_sunset(Config config);
