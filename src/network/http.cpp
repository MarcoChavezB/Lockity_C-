#include "http.h"

HTTPClient httpClient;

const char* baseUrl = "http://64.23.237.187:8003/api";
const char* auth_header = "EbW(.)(.)fYcV%_p+g=,pu0;0#9duJbhGhB===D";

URLRoutes urls = {
  String(baseUrl) + "/locker-config/",
  String(baseUrl) + "/locker-config/store-log",
  String(baseUrl) + "/locker-config/schedules/" + serial_number, // obtener los hoarios del locker
  String(baseUrl) + "/locker-config/" + serial_number + "/", // + compartment_number + status [open, closed, error, maintenance]
  String(baseUrl) + "/locker-config/still-open-alert/" + serial_number + "/" // compartment_number
};

bool still_open_alert(uint8_t compartment_number){
  String url = urls.still_open_alert + String(compartment_number);
  httpClient.begin(url);
  httpClient.addHeader("x-iot-key", auth_header);
  Serial.println("Conectando a: " + url);
  int httpCode = httpClient.POST("");

    if(httpCode != 200) {
        Serial.println("Error al enviar alerta: " + String(httpCode));
        httpClient.end();
        return false;
    }
    String payload = httpClient.getString();
    Serial.println("Respuesta: " + payload);    
    return true;
}

bool change_status_drawer(uint8_t compartent_number, String status){
    String url = urls.change_drawer_state + String(compartent_number) + "/" + status;
    httpClient.begin(url);
    httpClient.addHeader("x-iot-key", auth_header);
    Serial.println("Conectando a: " + url);
    int httpCode = httpClient.PUT("");
    
    if(httpCode != 200) {
        Serial.println("Error al cambiar el estado del cajón: " + String(httpCode));
        httpClient.end();
        return false;
    }
    String payload = httpClient.getString();
    Serial.println("Respuesta: " + payload);    
    return true;
}

bool publish_toggle_log(String base64Pic, int userId, int compartment_number, String action) {
    StaticJsonDocument<16384> doc;
    JsonArray logs = doc.createNestedArray("logs");
    JsonObject log = logs.createNestedObject();

    String timeStamp = get_time_stamp(); 
    String fileName = "camera-picture-" + serial_number + "-" + timeStamp + ".png";

    log["image_base_64"] = base64Pic;
    log["file_name"] = fileName;
    log["serial_number"] = serial_number;
    log["user_id"] = userId;
    log["compartment_number"] = compartment_number;
    log["action"] = action;
    log["source"] = "physical";

    String requestBody;
    serializeJson(doc, requestBody);
    httpClient.begin(urls.public_logs);
    Serial.println("Conectando a: " + urls.public_logs);
    httpClient.addHeader("Content-Type", "application/json");
    httpClient.addHeader("x-iot-key", auth_header);
    Serial.println("Enviando JSON: " + requestBody);

    int httpCode = httpClient.POST(requestBody);
    Serial.println("Código de respuesta: " + String(httpCode));
    Serial.println("Respuesta: " + httpClient.getString());
    httpClient.end();
    
    return httpCode == 200 || httpCode == 201;
}

bool get_schedules() {
    httpClient.begin(urls.get_schedules);
    Serial.println("Conectando a: " + urls.get_schedules);
    httpClient.addHeader("x-iot-key", auth_header);
    int httpCode = httpClient.GET();
    String payload = httpClient.getString();

    if (httpCode != 200) {
        Serial.println("Error al obtener horarios: " + String(httpCode));
        httpClient.end();
        return false;
    }

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.println("Error al parsear JSON: " + String(error.c_str()));
        httpClient.end();
        return false;
    }

    JsonArray arr = doc["data"]["schedules"].as<JsonArray>();
    scheduleCount = 0;

    for (JsonObject obj : arr) {
        if (scheduleCount >= MAX_SCHEDULES) {
            Serial.println("Demasiados horarios, algunos fueron ignorados.");
            break;
        }

        schedules[scheduleCount].id = obj["id"];
        schedules[scheduleCount].day_of_week = obj["day_of_week"] | "";
        schedules[scheduleCount].start_time = obj["start_time"] | "";
        schedules[scheduleCount].end_time = obj["end_time"] | "";
        schedules[scheduleCount].repeat_schedule = obj["repeat_schedule"];
        schedules[scheduleCount].schedule_date = obj["schedule_date"] | "";

        scheduleCount++;
    }

    httpClient.end();
    return true;
}


bool get_initial_config(){    
    httpClient.begin(urls.initial_config + serial_number);
    Serial.println("Conectando a: " + urls.initial_config + serial_number);
    httpClient.addHeader("x-iot-key", auth_header);
    int httpCode = httpClient.GET();

    String payload = httpClient.getString();
    
    Serial.println("response: " + payload);

    if (httpCode == 200) {
      parse_initial_config(payload);
      save_config_to_SPIFFS(payload);
    }else {
        Serial.println(httpCode);
        return false;
    }
    
    httpClient.end();
    return true;
}
