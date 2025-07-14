#include "spiffs.h"

// Función para parsear los topics desde el JSON recibido de Configuración Inicial
void parse_initial_config(const String& payload) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);

  if (err) {
    Serial.print("Error al parsear JSON: ");
    Serial.println(err.c_str());
    return;
  }

  JsonObject config = doc["initial_config"];
  idLocker = config["id_locker"].as<String>();

  // parse topics
  JsonObject topicsObj = config["topics"].as<JsonObject>();
  parse_topics(topicsObj);

  // parse users
  JsonArray users = config["users"];
  usuarioCount = 0;

  for (JsonObject user : users) {
    if (usuarioCount >= 5) break;
    
    usuarios[usuarioCount].id_usuario = user["id_usuario"].as<String>();
    usuarios[usuarioCount].name = user["nombre_usuario"].as<String>();

    JsonArray cajonesArray = user["cajones_usuario"].as<JsonArray>();
    usuarios[usuarioCount].cajon_count = 0;

    for (int i = 0; i < cajonesArray.size() && i < MAX_CAJONES; i++) {
        usuarios[usuarioCount].cajones_usuario[i] = cajonesArray[i].as<String>();
        usuarios[usuarioCount].cajon_count++;
    }
    
    Serial.printf("Usuario %s tiene %d cajones:\n", usuarios[usuarioCount].id_usuario.c_str(), usuarios[usuarioCount].cajon_count);
    for (int i = 0; i < usuarios[usuarioCount].cajon_count; i++) {
        Serial.println("  - " + usuarios[usuarioCount].cajones_usuario[i]);
    }


    usuarioCount++;
  }

  // Debug
  Serial.println("ID Locker: " + idLocker);
  Serial.println("Topics recibidos:");
  for (int i = 0; i < topicsCount; i++) {
    Serial.printf("  %s : %s\n", topics[i].key.c_str(), topics[i].value.c_str());
  }
}



void save_config_to_SPIFFS(const String &payload) {
  File file = SPIFFS.open("/config.json", FILE_WRITE);
  if(!file) {
    Serial.println("Error abriendo archivo para escribir");
    return;
  }

  file.print(payload);
  file.close();
  Serial.println("Configuración guardada en SPIFFS");
}

bool load_config() {
  if (!SPIFFS.exists("/config.json")) {
    return false;
  }

  File file = SPIFFS.open("/config.json", FILE_READ);
  if (!file) {
    Serial.println("Error abriendo archivo para leer");
    return false;
  }

  String payload = file.readString();
  file.close();

  parse_initial_config(payload); 

  return true;
}

bool delete_config() {
  if (SPIFFS.exists("/config.json")) {
    if (SPIFFS.remove("/config.json")) {
      Serial.println("Archivo de configuración eliminado");
      return true;
    } else {
      Serial.println("No se pudo eliminar el archivo");
      return false;
    }
  } else {
    Serial.println("Archivo no existe");
    return false;
  }
}


void print_config_to_serial() {
  Serial.println("===== Configuración Inicial =====");

  // Mostrar ID del Locker
  Serial.print("ID Locker: ");
  Serial.println(idLocker);

  // Mostrar Topics
  Serial.println("Topics:");
  for (int i = 0; i < topicsCount; i++) {
    Serial.print("  ");
    Serial.print(topics[i].key);
    Serial.print(" => ");
    Serial.println(topics[i].value);
  }

    Serial.println("Usuarios:");
    for (int i = 0; i < usuarioCount; i++) {
        Serial.println("  ID Usuario: " + usuarios[i].id_usuario);
        Serial.println("  Nombre: " + usuarios[i].name);
        Serial.print("  Cajones: ");
        for (int j = 0; j < usuarios[i].cajon_count; j++) {
            Serial.print(usuarios[i].cajones_usuario[j]);
            if (j < usuarios[i].cajon_count - 1) {
                Serial.print(", ");
            }
        }
        Serial.println();
        Serial.println("---------------------");
    }

  Serial.println("================================");
}
