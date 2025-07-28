#include "http.h"

HTTPClient httpClient;

const char* baseUrl = "http://64.23.237.187:8003/api";
const char* auth_header = "EbW(.)(.)fYcV%_p+g=,pu0;0#9duJbhGhB===D";

URLRoutes urls = {
  String(baseUrl) + "/locker-config/",
  String(baseUrl) + "/locker-config/store-log"
};

bool publish_toggle_log(String base64Pic, int userId, int compartment_number, String action) {
    String timeStamp = get_time_stamp(); 
    String fileName = "camera-picture-" + serial_number + "-" + timeStamp + ".png";

    String jsonBody = 
        "{\"logs\":[{"
        "\"image_base_64\":\"" + base64Pic + "\","
        "\"file_name\":\"" + fileName + "\","
        "\"serial_number\":\"" + serial_number + "\","
        "\"user_id\":" + String(userId) + ","
        "\"compartment_number\":" + String(compartment_number) + ","
        "\"action\":\"" + action + "\","
        "\"source\":\"physical\""
        "}]}";

    httpClient.begin(urls.public_logs);
    Serial.println("Conectando a: " + urls.public_logs);
    httpClient.addHeader("Content-Type", "application/json");
    httpClient.addHeader("x-iot-key", auth_header);

    int httpCode = httpClient.POST(jsonBody);
    Serial.println("Código de respuesta: " + String(httpCode));
    Serial.println("Respuesta: " + httpClient.getString());
    httpClient.end();
    return httpCode == 200 || httpCode == 201;
}



bool get_initial_config(){
    // checa si ya hay configuracion guardada
    if(load_config()) {
        Serial.println("Configuración ya cargada desde SPIFFS.");
        return true;
    }
    
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