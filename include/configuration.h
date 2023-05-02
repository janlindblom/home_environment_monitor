// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <LittleFS.h>

#include "configuration_types.h"

const char json_config[] = "config.json";

bool   configured();
bool   configuration_loaded();
void   load_configuration();
Config get_config();
