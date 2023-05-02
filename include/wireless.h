// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <BTstackLib.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <inttypes.h>

#include "arduino_secrets.h"
#include "configuration_types.h"

#ifndef STASSID
#  define STASSID "no-network"
#  define STAPSK "no-password"
#endif

enum WiFiSignal { AMAZING, GREAT, GOOD, OK, BAD, UNUSABLE };

void control_wireless(Config configuration, void (*callback)(BLEAdvertisement* bleAdvertisement));

void    connect_network();
void    disconnect_network();
void    set_network_disconnected();
bool    wifi_ap_configured();
bool    network_connected();
bool    network_disconnected();
bool    network_setup_running();
uint8_t wifi_signal_rating(int rssi);
void    print_wifi_status(U8G2 u8g2);

void configure_bluetooth(void (*callback)(BLEAdvertisement* bleAdvertisement));
bool bluetooth_configured();
bool bluetooth_configuring();
void set_bluetooth_configured(bool val);
void set_bluetooth_configuring(bool val);
bool bluetooth_scanning();
void ble_start_scanning();
void ble_stop_scanning();
void control_bluetooth_scanning();
void print_bluetooth_status(U8G2 u8g2);
void advertisementCallback(BLEAdvertisement* adv);