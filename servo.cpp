#include "servo.h"

StaticJsonDocument<256> servoDoc;

const int servo_pins[3] = {13, 12, 14};
bool drawer_states[3] = {false, false, false};
const int led_pins[3] = {26, 25, 32}; 

void setup_servo() {
  for (int i = 0; i < 3; i++) {
    pinMode(led_pins[i], OUTPUT);
    pinMode(servo_pins[i], OUTPUT);
    digitalWrite(servo_pins[i], LOW);
    close_drawer(i); 
  }
}

void set_servo_angle(int servo_index, int angle) {
  int pulseWidth = map(angle, 0, 180, 500, 2500);
  int servoPin = servo_pins[servo_index];

  for (int i = 0; i < 25; i++) {
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(servoPin, LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}


void manage_servo_payload(String payload ){
    if(!json_parser(payload, servoDoc)){
        Serial.println("Error al parsear JSON del servo");
        return;
    }
    
    uint8_t action = servoDoc["valor"] | 0;
    String drawer_id = servoDoc["id_drawer"].as<String>();
    String user_id = servoDoc["id_usuario"].as<String>();
    
    if(action == 1){
        if(!is_drawer_owner(user_id, drawer_id)){
            serial.println("Usuario no autorizado para abrir el cajón: " + drawer_id);
            return;
        } 
        Serial.println("Abriendo cajon: " + drawer_id);
        int servo_index = drawer_id.toInt() - 1; 
        Serial.println("Servo index: " + String(servo_index));
        open_drawer(servo_index);
    } else{
        if(!is_drawer_owner(user_id, drawer_id)){
            Serial.println("Usuario no autorizado para cerrar el cajón: " + drawer_id);
            return;
        }
        
        Serial.println("Cerrando cajon: " + drawer_id);
        int servo_index = drawer_id.toInt() - 1; 
        close_drawer(servo_index);
    }
    Serial.println("Acción del servo completada");
}

void open_drawer(uint8_t servo_index) {
  set_servo_angle(servo_index, 180); 
  digitalWrite(led_pins[servo_index], HIGH);
  drawer_states[servo_index] = true;
}

void close_drawer(uint8_t servo_index) {
  set_servo_angle(servo_index, 0);
  digitalWrite(led_pins[servo_index], LOW);
  drawer_states[servo_index] = false;
}


void fingerprint_shearch(uint16_t finger_id){
    for (int i = 0; i < usuarioCount; i++){
        if (usuarios[i].id_usuario.toInt() == finger_id) {
            for (int j = 0; j < usuarios[i].cajon_count; j++) {
                uint8_t drawer_index = usuarios[i].cajones_usuario[j].toInt() - 1;
                if (is_drawer_open(drawer_index)) {
                    close_drawer(drawer_index);
                } else {
                    open_drawer(drawer_index);
                }
            }
        }
    }
}


bool is_drawer_open(uint8_t servo_index) {
  return drawer_states[servo_index];
}
