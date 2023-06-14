// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "configuration_types.h"

const char json_config[] PROGMEM = "config.json";

bool   configured();
bool   configuration_loaded();
void   load_configuration();
void   load_config_file();
Config get_config();
