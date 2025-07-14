#include "http.h"

HTTPClient httpClient;

const char* baseUrl = "http://64.23.237.187:8003/api";
const char* auth_header = "EbW(.)(.)fYcV%_p+g=,pu0;0#9duJbhGhB===D";

URLRoutes urls = {
  String(baseUrl) + "/locker-config/",
};

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