// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiNTP.h>
#include <WiFiUdp.h>

extern volatile bool wifi_ap_configured;
extern volatile bool network_connected;

void connect_network();
