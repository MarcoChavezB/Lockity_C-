#pragma once

#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include <network/http.h>
#include <sensors/clock.h>
#include <system.h>

void websocket_setup();
void websocket_loop();
void send_take_picture();
