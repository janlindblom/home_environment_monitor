// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>

#define splash_logo_width 128
#define splash_logo_height 24

static unsigned char splash_logo_bits[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x80, 0xff, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0xe0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x0f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0xfc, 0x81, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0xfc, 0xbe, 0x1f, 0x90, 0x07, 0xc0, 0x01, 0xe6,
    0xe0, 0x03, 0xe0, 0x01, 0x03, 0x7c, 0x00, 0x3c, 0x7e, 0xbf, 0x3f, 0xd0,
    0x18, 0x30, 0x06, 0x16, 0x33, 0x06, 0x16, 0x03, 0x03, 0x86, 0x01, 0x61,
    0x7e, 0xbf, 0x3f, 0x10, 0x10, 0x08, 0x08, 0x06, 0x06, 0x0c, 0x06, 0x04,
    0x03, 0x01, 0x82, 0x81, 0x7f, 0xbf, 0x7f, 0x10, 0x20, 0x00, 0x08, 0x06,
    0x04, 0x08, 0x06, 0x04, 0x03, 0x00, 0x86, 0x01, 0x7f, 0xbf, 0x7f, 0x10,
    0x20, 0x00, 0x18, 0x06, 0x0c, 0x18, 0x06, 0x0c, 0x43, 0x00, 0x84, 0x01,
    0x7f, 0xbf, 0x7f, 0x10, 0x60, 0x00, 0x18, 0x06, 0x0c, 0x18, 0x06, 0x0c,
    0x43, 0x00, 0x0c, 0x02, 0x7f, 0xbf, 0x7f, 0x10, 0x60, 0x00, 0x18, 0x06,
    0x0c, 0x18, 0x06, 0x0c, 0x43, 0x00, 0x0c, 0x0c, 0x7f, 0xbf, 0x7f, 0x10,
    0x60, 0xf0, 0x1b, 0x06, 0x0c, 0x18, 0x06, 0x0c, 0x43, 0x00, 0x0c, 0x30,
    0x7f, 0xbf, 0x7f, 0x10, 0x60, 0x08, 0x18, 0x06, 0x0c, 0x18, 0x06, 0x0c,
    0x43, 0x00, 0x0c, 0x40, 0x7e, 0xbf, 0x3f, 0x10, 0x60, 0x04, 0x18, 0x06,
    0x0c, 0x18, 0x06, 0x0c, 0x43, 0x00, 0x04, 0x80, 0x7e, 0xbf, 0x3f, 0x10,
    0x60, 0x04, 0x18, 0x06, 0x0c, 0x18, 0x06, 0x0c, 0x83, 0x00, 0x04, 0x80,
    0x7c, 0xbf, 0x1f, 0x10, 0x60, 0x04, 0x18, 0x06, 0x0c, 0x18, 0x06, 0x0c,
    0x83, 0x01, 0x82, 0x80, 0xf8, 0xe0, 0x0f, 0x10, 0x60, 0x08, 0x1a, 0x06,
    0x0c, 0x18, 0x06, 0x0c, 0x03, 0x03, 0x01, 0x01, 0xf0, 0xff, 0x07, 0x10,
    0x60, 0xf0, 0x19, 0x06, 0x0c, 0x18, 0x06, 0x0c, 0x03, 0xfc, 0x00, 0x3e,
    0xe0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
