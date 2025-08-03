#include "spiffs.h"

// Función para parsear los topics desde el JSON recibido de Configuración Inicial
void parse_initial_config(const String& payload) {
    Serial.println("📄 Leyendo contenido desde SPIFFS:");
    Serial.println(payload);

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
    usuarios[usuarioCount].role = user["rol"].as<String>();
    
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

    if(usuarios[usuarioCount].role == "user"){
      usuarioUsersCount ++;
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

bool exist_initial_config(){
  if(SPIFFS.exists("/config.json")){
    Serial.println("Si hay configuracion inicial ++++++++++++++");
    return true;
  }
  Serial.println("No hay configuracion incial *************8");
  return false;
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

  if (SPIFFS.exists("/config.json")) {
    File file = SPIFFS.open("/config.json", FILE_READ);
    if (file) {
      Serial.println("📄 Contenido crudo de config.json:");
      while (file.available()) {
        Serial.write(file.read()); 
      }
      file.close();
    } else {
      Serial.println("❌ No se pudo abrir el archivo config.json");
    }
  } else {
    Serial.println("⚠️ El archivo config.json no existe");
  }
    Serial.println("===== Configuración fingerprints =====");

  if(SPIFFS.exists("/fingerprints.json")){
    File file = SPIFFS.open("/fingerprints.json", FILE_READ);
    if (file) {
      Serial.println("📄 Contenido crudo de fingerprints.json:");
      while (file.available()) {
        Serial.write(file.read()); 
      }
      file.close();
    } else {
      Serial.println("❌ No se pudo abrir el archivo fingerprints.json");
    }
    } else {
        Serial.println("⚠️ El archivo fingerprints.json no existe");
  }

  Serial.println("\n================================");
}



bool save_mapping_fingerprint(uint8_t fingerprint_id, uint16_t user_id, uint8_t drawer_id) {
  // Leer archivo existente
  File file = SPIFFS.open("/fingerprints.json", "r");
  DynamicJsonDocument doc(2048);
  if (file) {
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      Serial.println("Error al leer JSON: " + String(error.c_str()));
    }
    file.close();
  }

  // Agregar nuevo mapeo
  JsonObject nuevo = doc.createNestedObject();
  nuevo["fingerprint_id"] = fingerprint_id;
  nuevo["user_id"] = user_id;
  nuevo["drawer_id"] = drawer_id;

  // Guardar archivo de nuevo
  file = SPIFFS.open("/fingerprints.json", "w");
  if (!file) {
    Serial.println("No se pudo abrir el archivo para escribir");
    return false;
  }
  serializeJson(doc, file);
  file.close();

  Serial.println("✅ Mapeo guardado en SPIFFS.");
  return true;
}

uint8_t get_free_fingerprint_id() {
  for (uint8_t i = 1; i <= 127; i++) {
    if (!is_used_fingerprit(i)) {
      return i;
    }
  }
  return 0;
}


bool is_used_fingerprit(uint8_t fingerprint_id) {
  File file = SPIFFS.open("/fingerprints.json", "r");
  if (!file) return false;

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, file);
  file.close();

  for (JsonObject obj : doc.as<JsonArray>()) {
    if ((uint8_t)obj["fingerprint_id"] == fingerprint_id) {
      return true;
    }
  }
  return false;
}

