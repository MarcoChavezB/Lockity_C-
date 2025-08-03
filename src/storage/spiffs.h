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
bool save_mapping_fingerprint(uint8_t fingerprint_id, uint16_t user_id, uint8_t drawer_id);
bool is_used_fingerprit(uint8_t fingerprint_id);
uint8_t get_free_fingerprint_id();
bool exist_initial_config();


// variables en memoria
extern String wifi_password;
