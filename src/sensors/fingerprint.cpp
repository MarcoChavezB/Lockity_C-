#include "fingerprint.h"

HardwareSerial serial(2);
Adafruit_Fingerprint finger(&serial);
const char* fingerprint_topic = "/command/fingerprint";

unsigned long dispFpStateStart = 0;

enum DispFpState {
  DISP_NONE,
  DISP_SUCCESS,
  DISP_ERROR
};

DispFpState dispFpState = DISP_NONE;
extern volatile uint8_t attemp_fingerprint;
uint8_t error_attemp_fingerprint;

bool fingerprint_init(){
    finger.begin(57600);
    
    if(!finger.verifyPassword()) {
        Serial.println("❌ Error: Sensor de huellas no detectado o contraseña incorrecta.");
        return false;
    }
    
    Serial.println("✅ Sensor de huellas inicializado correctamente.");
    return true;
}

unsigned long fingerprintLastAttempt = 0;
const unsigned long fingerprintInterval = 2000; // intervalo entre intentos

void fingerprint_loop() {
  if (millis() - fingerprintLastAttempt < fingerprintInterval) return; // espera

  fingerprintLastAttempt = millis();

  int result = finger.getImage();
  if (result != FINGERPRINT_OK) return;

  result = finger.image2Tz();
  if (result != FINGERPRINT_OK) return;

  result = finger.fingerSearch();
  if (result == FINGERPRINT_OK) {
    Serial.print("✅ Huella detectada. ID: ");
    Serial.println(finger.fingerID);
    attemp_fingerprint = 1;
    fingerprint_shearch(finger.fingerID);
  } else {
    Serial.println("❌ Huella no encontrada.");
    attemp_fingerprint = 2;
    error_attemp_fingerprint ++;
    if(error_attemp_fingerprint > 2){
      send_take_picture();
      error_attemp_fingerprint = 0;
    }
    
    // Actualiza estado visual sin delay
    dispFpState = DISP_ERROR;
    dispFpStateStart = millis();
  }
}

// Función para registrar huella con dos toques
uint8_t getFingerprintEnroll(uint8_t id_param) {
  int p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return false;

  Serial.println("Retira el dedo...");
  mqtt_publish(
    get_topic("fingerprint").c_str(), 
    build_fingerprint_message(id_param, 1, "put", "waiting", "Remove your finger...").c_str()
  );

  unsigned long start = millis();
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    if (millis() - start > 10000) return false; // Timeout 10s
    delay(100);
  }

  Serial.println("Coloca el mismo dedo nuevamente...");
  mqtt_publish(
    get_topic("fingerprint").c_str(), 
    build_fingerprint_message(id_param, 1, "put", "waiting", "Place the same finger again...").c_str()
  );

  start = millis();
  while (finger.getImage() != FINGERPRINT_OK) {
    if (millis() - start > 10000) return false; // Timeout 10s
    delay(100);
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return false;

  p = finger.createModel();
  if (p != FINGERPRINT_OK) return false;

  p = finger.storeModel(id_param);
  return p == FINGERPRINT_OK;
}

void fingerprint_create(uint16_t user_id, uint8_t drawer_id) {
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ No se pudo montar SPIFFS.");
    return;
  }

  uint8_t fingerprint_id = get_free_fingerprint_id();

  if (fingerprint_id == 0) {
    Serial.println("❌ No hay espacio disponible para más huellas.");
    return;
  }

  Serial.println("🆕 Iniciando registro de huella:");
  Serial.println("👤 Usuario: " + String(user_id));
  Serial.println("📦 Cajón: " + String(drawer_id));
  Serial.println("🆔 Fingerprint ID asignado: " + String(fingerprint_id));
  
  mqtt_publish(
    get_topic("fingerprint").c_str(),
    build_fingerprint_message(fingerprint_id, 1, "put", "waiting", "Place your finger to start registration...").c_str()
  );

  unsigned long startTime = millis();
  const unsigned long timeout = 10000;

  while (true) {
    if (millis() - startTime > timeout) {
      Serial.println("⏳ Tiempo de registro agotado.");
      mqtt_publish(
        get_topic("fingerprint").c_str(),
        build_fingerprint_message(fingerprint_id, 0, "put", "timeout", "Registration time expired. Try again.").c_str()
      );
      break;
    }

    if (finger.getImage() == FINGERPRINT_OK) {
      Serial.println("👉 Huella detectada. Iniciando proceso...");

      mqtt_publish(
        get_topic("fingerprint").c_str(),
        build_fingerprint_message(fingerprint_id, 1, "put", "waiting", "Fingerprint detected. Starting registration...").c_str()
      );

      if (getFingerprintEnroll(fingerprint_id)) {
        Serial.println("✅ Huella registrada correctamente.");

        if (save_mapping_fingerprint(fingerprint_id, user_id, drawer_id)) {
          mqtt_publish(
            get_topic("fingerprint").c_str(),
            build_fingerprint_message(fingerprint_id, 1, "confirm", "success", "Fingerprint registered and mapping saved.").c_str()
          );
        } else {
          mqtt_publish(
            get_topic("fingerprint").c_str(),
            build_fingerprint_message(fingerprint_id, 1, "confirm", "partial", "Fingerprint registered but mapping was not saved.").c_str()
          );
        }
      } else {
        Serial.println("❌ Error al registrar la huella.");
        mqtt_publish(
          get_topic("fingerprint").c_str(),
          build_fingerprint_message(fingerprint_id, 0, "put", "fail", "Error registering fingerprint. Try again.").c_str()
        );
      }

      break;
    }

    delay(100);
  }
}

void fingerprint_delete_all() {
  int result = finger.emptyDatabase();

  if (result == FINGERPRINT_OK) {
  
    if (SPIFFS.exists("/fingerprints.json")){
        SPIFFS.remove("/fingerprints.json");
    }
  
    Serial.println("✅ Todas las huellas fueron eliminadas.");
    mqtt_publish(
      get_topic("fingerprint").c_str(), 
      build_fingerprint_message(0, 1, "delete_all", "success", "All fingerprints deleted").c_str()
    );
  } else {
    Serial.print("❌ Error al borrar todas las huellas. Código: ");
    Serial.println(result);
    mqtt_publish(
      get_topic("fingerprint").c_str(), 
      build_fingerprint_message(0, 0, "delete_all", "fail", "Failed to delete all fingerprints").c_str()
    );
  }
}

bool search_fingerprint_mapping(uint16_t fingerprint_id, uint16_t &user_id_out, uint8_t &drawer_id_out) {
  if (!SPIFFS.begin(true)) {
      Serial.println("❌ Error montando SPIFFS en search_fingerprint_mapping");
      return false;
  }

  File file = SPIFFS.open("/fingerprints.json", "r");
  if (!file) {
      Serial.println("❌ No se pudo abrir fingerprints.json");
      return false;
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
      Serial.print("❌ Error parseando fingerprints.json: ");
      Serial.println(error.c_str());
      return false;
  }

  for (JsonObject obj : doc.as<JsonArray>()) {
      uint16_t fid = obj["fingerprint_id"].as<uint16_t>();
      Serial.print("Leyendo fingerprint_id: ");
      Serial.println(fid);
      if (fid == fingerprint_id) {
        user_id_out = obj["user_id"].as<uint16_t>();
        drawer_id_out = obj["drawer_id"].as<uint8_t>();
        Serial.print("✔ Encontrado: user_id = ");
        Serial.print(user_id_out);
        Serial.print(", drawer_id = ");
        Serial.println(drawer_id_out);
        return true;
      }
  }

  Serial.println("❌ No se encontró el fingerprint_id en fingerprints.json");
  return false;
}

/**
 * @brief Construye un mensaje JSON para operaciones de huella digital.
 *
 * Esta función genera un `String` con formato JSON que representa el estado
 * de un proceso relacionado con la configuración o lectura de huellas digitales.
 *
 * @param id        ID del usuario (generalmente entre 1 y 127).
 * @param config    Valor de configuración (1 para éxito, 0 para error).
 * @param stage     Etapa del proceso ("put", "remove", "confirm", etc.).
 * @param status    Estado del proceso ("waiting", "success", "fail", etc.).
 * @param message   Mensaje descriptivo que se enviará al cliente.
 * 
 * @return `String` con el JSON formateado que representa el estado del proceso.
 *
 * @example
 * String msg = build_fingerprint_message(3, 1, "put", "waiting", "Put the finger...");
 * mqtt_publish("/topic", msg.c_str());
 */
String build_fingerprint_message(uint8_t id, int config, const char* stage, const char* status, const char* message) {
  return
    String("{\n")
    + "  \"config\": " + String(config) + ",\n"
    + "  \"user_id\": \"" + String(id) + "\",\n"
    + "  \"stage\": \"" + stage + "\",\n"
    + "  \"status\": \"" + status + "\",\n"
    + "  \"message\": \"" + message + "\"\n"
    + "}";
}
