// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "HomeEnvironmentMonitor.h"
#include "arduino_secrets.h"
#include "networking.h"

static const unsigned char PROGMEM logo16_glcd_bmp[] = {0,   192, 1,   192, 1,   192, 3,   224, 243, 224, 254,
                                                        248, 126, 254, 51,  159, 31,  252, 13,  112, 27,  160,
                                                        63,  224, 63,  240, 124, 240, 112, 112, 0,   48};

extern volatile bool network_connected;
volatile bool        network_time_set = false;

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid     = STASSID;
const char* password = STAPSK;
#ifdef SECSSID
const char* ssid2     = SECSSID;
const char* password2 = SECPSK;
#endif
WiFiMulti multi;

void setup() {
  if (!Serial) {
    Serial.begin(115200);
  }

  connect_network();

  // BTstack.setup();

  CONDITIONAL_SERIAL_PRINTLN("Initialising display...");
  delay(250); // wait for the OLED to power up
  display.begin(I2C_ADDRESS, true);
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();

  // display.drawBitmap(30, 16, logo16_glcd_bmp, 16, 16, 1);
  display.drawBitmap(display.width() / 2 - LOGO16_GLCD_WIDTH / 2, display.height() / 2 - LOGO16_GLCD_HEIGHT / 2,
                     logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, 1);
  display.display();
}

void loop() {
  delay(5000);

  if (WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_DISCONNECTED) {
    network_connected = false;
  }

  if (network_connected) {
    CONDITIONAL_SERIAL_PRINT("Connected to network: ");
    CONDITIONAL_SERIAL_PRINTLN(WiFi.SSID());

    if (!network_time_set) {
      CONDITIONAL_SERIAL_PRINTLN("Setting time from network...");
      NTP.begin(NTP_SERVER1, NTP_SERVER2, NTP_TIMEOUT);
      if (NTP.waitSet()) {
        network_time_set = true;
      }
    }
    print_time();

  } else {
    connect_network();
  }
}

void print_time() {
  if (network_time_set) {
    time_t    now = time(nullptr);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    if (now > 0) {
      CONDITIONAL_SERIAL_PRINT("Current time: ");
      CONDITIONAL_SERIAL_PRINTLN(asctime(&timeinfo));
    }
  }
}
