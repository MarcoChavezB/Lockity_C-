#pragma once

#include <display/display.h>
#include <network/http.h>
#include <network/mqtt.h>
#include <network/wifi.h>
#include <sensors/fingerprint.h>
#include <storage/spiffs.h>
#include <motor/servo.h>
#include <sensors/clock.h>
#include <network/websockets/websocket.h>

void init_system();
bool connect_wifi();
bool fetch_locker_config();
void setup_network_services();
bool check_internet_access();
void reset_system_data();
void listFiles();

extern bool has_wifi;
extern bool already_mqtt_setup;
extern bool has_initial_config;
extern bool camera_connected;
extern uint8_t attemp_fingerprint;


