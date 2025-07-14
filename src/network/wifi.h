#pragma once
#include <WiFiManager.h>
#include <display/display.h>

bool wifi_connect();
bool share_wifi();
void reset_credentials();
void configModeCallback(WiFiManager *myWiFiManager);