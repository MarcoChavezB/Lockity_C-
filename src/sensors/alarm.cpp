#include "alarm.h"

int alarm_pin = 27;

void setup_alarm() {
    pinMode(alarm_pin, OUTPUT);
    digitalWrite(alarm_pin, LOW);
}

void turn_off_alarm(){
    digitalWrite(alarm_pin, LOW);
    Serial.println("Alarma apagada");
}

void turn_on_alarm() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(alarm_pin, HIGH);
    delay(200);
    digitalWrite(alarm_pin, LOW);
    delay(200);
  }
}
