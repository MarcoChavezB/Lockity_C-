#include <Arduino.h>
#include <utils/json.h>
#include <middleware/control.h>

void setup_servo();
void open_drawer(uint8_t servo_index);
void close_drawer(uint8_t servo_index);
void set_servo_angle(int servo_index, int angle);
void manage_servo_payload(String payload );
void fingerprint_shearch(uint16_t finger_id);
bool is_drawer_open(uint8_t servo_index);