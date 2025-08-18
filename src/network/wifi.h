#pragma once

#include <WiFiManager.h>
#include <display/display.h>
#include <Preferences.h>
#include <FS.h>
#include <SPIFFS.h>
#include <network/websockets/websocket.h>
#include <WebServer.h>

enum WifiState {
    WIFI_IDLE,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_FAILED,
    WIFI_TIMEOUT
};

bool wifi_connect(); 
void setup_ap();
void reset_credentials();
void configModeCallback(WiFiManager *myWiFiManager);
void display_wifi_status(WifiState state);

extern bool has_saved_credentials;

