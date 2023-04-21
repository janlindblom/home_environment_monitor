// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "networking.h"

volatile bool wifi_ap_configured = false;
volatile bool network_connected  = false;

void connect_network() {
  CONDITIONAL_SERIAL_PRINTLN("Connecting to WiFi...");
  WiFi.setHostname("envmon");
  auto cnt = WiFi.scanNetworks();
#ifdef STASSID
  if (multi.addAP(ssid, password)) {
#  ifdef SECSSID
    if (multi.addAP(ssid2, password2)) {
#  endif
      wifi_ap_configured = true;
#  ifdef SECSSID
    }
#  endif
  }
#endif
  if (multi.run() != WL_CONNECTED) {
    network_connected = false;
  } else {
    network_connected = true;
  }
}
