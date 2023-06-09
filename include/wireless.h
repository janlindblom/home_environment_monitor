// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <BTstackLib.h>
#include <U8g2lib.h>
#include <WiFi.h>

enum WiFiSignal { AMAZING, GREAT, GOOD, OK, BAD, UNUSABLE };

void control_wireless();

void    connect_network();
void    disconnect_network();
bool    wifi_ap_configured();
bool    network_connected();
bool    network_disconnected();
bool    network_setup_running();
uint8_t wifi_signal_rating(int rssi);
void    print_wifi_status();

void configure_bluetooth();
bool bluetooth_configured();
bool bluetooth_configuring();
bool bluetooth_scanning();
void ble_start_scanning();
void ble_stop_scanning();
void control_bluetooth_scanning();
void print_bluetooth_status();
void advertisementCallback(BLEAdvertisement* adv);
