#include "websocket.h"

extern WebServer server;      // Servidor HTTP clásico declarado en main.cpp
WebSocketsServer webSocket = WebSocketsServer(81);  // Puerto para WebSocket

String photoBase64Received = "";  
bool receivingPhoto = false;


// Evento de WebSocket clásico
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.printf("Cliente conectado: %u\n", num);
      camera_connected = true;
      break;
    case WStype_DISCONNECTED:
      Serial.printf("Cliente desconectado: %u\n", num);
      camera_connected = false;
      break;
    case WStype_TEXT: {
      String msg = String((char*)payload, length);
      if (msg == "START_PICTURE") {
        photoBase64Received = "";
        receivingPhoto = true;
        Serial.println("Iniciando recepción de foto...");
      }
      else if (msg == "END_PICTURE") {
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
    }
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_PONG:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

void websocket_setup() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", []() {
     server.send(200, "text/plain", "Hola mundo");
  });
}

void send_take_picture(){
    Serial.println("Enviando comando de tomar foto a WebSocket");
    webSocket.broadcastTXT("take_picture");
}

void websocket_loop() {
  webSocket.loop();
}
