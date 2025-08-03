#include "websocket.h"

// ======== OBJETOS DE SERVIDOR ========
WebServer serverSocket(80);
WebSocketsServer webSocket(80);

// ======== EVENTOS DEL WEBSOCKET ========
String photoBase64Received = "";  
bool receivingPhoto = false;

void websocket_event(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.printf("Cliente [%u] conectado\n", num);
      camera_connected = true;
      photoBase64Received = "";
      receivingPhoto = false;
      break;

    case WStype_DISCONNECTED:
      Serial.printf("Cliente [%u] desconectado\n", num);
      camera_connected = false;
      break;

    case WStype_TEXT: {
      String msg = String((char*)payload, length);
      if (msg == "start_photo") {
        photoBase64Received = "";
        receivingPhoto = true;
        Serial.println("Iniciando recepción de foto...");
      }
      else if (msg == "end_photo") {
        receivingPhoto = false;
        Serial.println("Recepción de foto terminada. enviando...");
        if(publish_toggle_log(photoBase64Received, pendingUserId, pendingCompartmentNumber, pendingAction)){
            delay(1000);
            String action = pendingAction == "opening" ? "open" : "closed";
            change_status_drawer(pendingCompartmentNumber, action);        
        }
      }
      else if (receivingPhoto) {
        photoBase64Received += msg;
      }
      break;
    }
  }
}

// ======== ENVÍO DE COMANDO AL ESP32-CAM ========
void send_take_picture() {
  if(!is_within_schedule()){
    Serial.println("❌ No se puede tomar una foto fuera del horario permitido.");
    return;
  }
  Serial.println("📤 Enviando comando: take_picture");
  webSocket.broadcastTXT("take_picture");
}

// ======== INICIALIZACIÓN DEL WEBSOCKET Y HTTP ========
void websocket_setup() {
  webSocket.begin();
  webSocket.onEvent(websocket_event);
  serverSocket.begin();
}

void websocket_loop() {
  webSocket.loop();
}
