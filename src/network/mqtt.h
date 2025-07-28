#pragma once
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h> 
#include <ArduinoJson.h>
#include "model.h"
#include "sensors/alarm.h"
#include "utils/json.h"
#include "sensors/fingerprint.h"
#include "motor/servo.h"
#include "network/http.h"
#include "logs/logs.h"

// Declaración del certificado (extern, definido en mqtt.cpp)
extern const char* ca_cert;

// Declaración de credenciales MQTT (extern)
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* mqtt_user;
extern const char* mqtt_password;

// Funciones públicas
void reconnect();
void mqtt_setup();
String get_topic(const String &key);
void mqtt_loop();
bool mqtt_publish(const char* topic, const char* payload);
String get_topic_key(const String& topicReceived);
void callback(char* topic, byte* payload, unsigned int length);

// Funciones de configuracion de topics http
void parse_topics(JsonObject& topicsObj);
String get_topic_value(const String& key);

// Cliente MQTT
extern WiFiClientSecure espClient;
extern PubSubClient client;

// variables de respuesta
using DocType = StaticJsonDocument<256>;
extern DocType responseFingerDoc;
