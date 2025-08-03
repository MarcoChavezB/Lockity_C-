#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <network/model.h>
#include <network/mqtt.h>
#include <storage/spiffs.h>

extern HardwareSerial serial;
extern Adafruit_Fingerprint finger;
extern String fingerprint_topic_str;
extern const char* fingerprint_topic;

extern String mqtt_message_put_finger;
extern String mqtt_message_error_invalid;
extern String mqtt_message_detected_finger;
extern String mqtt_message_successfully;
extern String mqtt_finger_status_remove;
extern String mqtt_finger_status_put;

bool fingerprint_init();
void fingerprint_loop();
void fingerprint_create(uint16_t user_id, uint8_t drawer_id);
void fingerprint_delete_all();
String build_fingerprint_message(uint8_t id, int config, const char* stage, const char* status, const char* message);
bool search_fingerprint_mapping(uint16_t fingerprint_id, uint16_t &user_id_out, uint8_t &drawer_id_out);
#endif
