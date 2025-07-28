#include "logs.h"
StaticJsonDocument<256> logDoc;

void toggle_log(String payload, String base64Image){
    if(!json_parser(payload, logDoc)){
        Serial.println("Error al parsear JSON del servo");
        return;
    }
    
    uint8_t action = logDoc["valor"] | 0;
    String drawer_id = logDoc["id_drawer"].as<String>();
    String user_id = logDoc["id_usuario"].as<String>();
    
    publish_toggle_log(base64Image, user_id.toInt(), user_id.toInt(), action == 1 ? "open" : "close");    
}
