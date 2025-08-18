#pragma once
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include <network/http.h>
#include <sensors/clock.h>
#include <network/model.h>
#include <system.h>


void websocket_loop();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void websocket_setup();
void send_take_picture();