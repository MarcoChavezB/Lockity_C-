#include "fingerprint.h"

HardwareSerial serial(2);
Adafruit_Fingerprint finger(&serial);
const char* fingerprint_topic = "/command/fingerprint";

bool fingerprint_init(){
    finger.begin(57600);
    
    if(!finger.verifyPassword()) {
        return false;
    }
    
    return true;
}


void fingerprint_loop(){
  int result = finger.getImage();
  if (result != FINGERPRINT_OK) return;

  result = finger.image2Tz();
  if (result != FINGERPRINT_OK) return;

  result = finger.fingerSearch();
  if (result == FINGERPRINT_OK) {
    Serial.print("✅ Huella detectada. ID: ");
    Serial.print(finger.fingerID);
    Serial.print(" | Confianza: ");
    Serial.println(finger.confidence);
    fingerprint_shearch(finger.fingerID);
    delay(1500);
  } else {
    Serial.println("❌ Huella no encontrada.");
    delay(1000);
  }
}


uint8_t getFingerprintEnroll(uint8_t id_param) {
  int p = -1;

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return false;

  Serial.println("Retira el dedo...");
  mqtt_publish(
    get_topic("fingerprint").c_str(), 
    build_fingerprint_message(id_param, 1, "put", "waiting", "Remove the finger...").c_str()
  );
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  Serial.println("Coloca el mismo dedo nuevamente...");
  mqtt_publish(
    get_topic("fingerprint").c_str(), 
    build_fingerprint_message(id_param, 1, "put", "waiting", "Put the same finger again...").c_str()
    );
  while (finger.getImage() != FINGERPRINT_OK) {
    delay(100);
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return false;

  p = finger.createModel();
  if (p != FINGERPRINT_OK) return false;

  p = finger.storeModel(id_param);
  return p == FINGERPRINT_OK;
}

void fingerprint_create(uint8_t id_param) {
  if (id_param == 0 || id_param > 127) {
    Serial.println("ID inválido. Intenta de nuevo.");
    mqtt_publish(
        get_topic("fingerprint").c_str(), 
        build_fingerprint_message(id_param, 0, "put", "fail", "Invalid ID. Please try again.").c_str()
    );
    return;
  }

  Serial.println("Coloca el dedo para comenzar el registro...");
  Serial.println(get_topic("fingerprint").c_str());
  // publicar mensajes en topic
    mqtt_publish(
        get_topic("fingerprint").c_str(),
        build_fingerprint_message(id_param, 1, "put", "waiting", "Put the finger to init the configuration...").c_str()
    );


  while (true) {
    if (finger.getImage() == FINGERPRINT_OK) {
      Serial.println("\nHuella detectada. Iniciando proceso...");
      
      mqtt_publish(
        get_topic("fingerprint").c_str(), 
        build_fingerprint_message(id_param, 1, "put", "waiting", "Finger detected. Init the process...").c_str()
      );
      
      if (getFingerprintEnroll(id_param)) {
        Serial.println("✅ Huella registrada correctamente.");
        mqtt_publish(
            get_topic("fingerprint").c_str(), 
            build_fingerprint_message(id_param, 1, "confirm", "success", "The finger was configured").c_str()
        );
        delay(1000);
      } else {
        Serial.println("❌ Error al registrar la huella.");
        mqtt_publish(
            get_topic("fingerprint").c_str(), 
            build_fingerprint_message(id_param, 0, "put", "fail", "An error occurred, please try again.").c_str()
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
