#pragma once
#include <Arduino.h>
#include <SPIFFS.h>
#include "network/model.h"
#include "network/mqtt.h"
#include <ArduinoJson.h>



void parse_initial_config(const String& payload);
void save_config_to_SPIFFS(const String &payload);
bool load_config();
bool delete_config();
void print_config_to_serial();


// variables en memoria
extern String wifi_password;