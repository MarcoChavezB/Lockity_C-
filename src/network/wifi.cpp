#include "wifi.h"
#include <WiFiManager.h>

const char* ap_ssid = "Lockity_camera_config";
const char* ap_password = "&E43+8kpG'sTbFq2)zw3RnLG2jqOKYrk:{#iLe]U6'+`Z*&@SG";

// Buffers para almacenar las credenciales
char userSSID[32] = "";
char userPassword[64] = "";

WiFiManager wm;
WiFiServer server(3333);

bool shouldSaveConfig = false;

// Callback llamado cuando se guardan nuevas credenciales
void saveConfigCallback() {
  Serial.println("Guardar configuración WiFi...");
  shouldSaveConfig = true;
}

void reset_credentials(){
    wm.resetSettings();
}

// Función para conectar a WiFi usando WiFiManager y capturar SSID y password
bool wifi_connect() {
  // Parámetro personalizado para capturar password
  WiFiManagerParameter custom_pass_param("pass", "WiFi Password", userPassword, 64);

  wm.addParameter(&custom_pass_param);
  wm.setSaveConfigCallback(saveConfigCallback);

  // Inicia el portal de configuración (abre AP si no tiene credenciales)
  if (!wm.autoConnect("Lockity_config")) {
    Serial.println("Fallo al conectar y configurar WiFi, reiniciando...");
    delay(3000);
    ESP.restart();
  }

  // Si llegamos aquí, estamos conectados a la WiFi
  Serial.println("Conectado a WiFi!");
  
  // Copiar SSID actual a userSSID
  strcpy(userSSID, WiFi.SSID().c_str());

  // Copiar password personalizado que ingresó el usuario (si se cambió)
  strcpy(userPassword, custom_pass_param.getValue());

  return true;
}

bool share_wifi() {
  Serial.println("Creando punto de acceso para compartir credenciales...");

  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("IP del AP: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  unsigned long startTime = millis();

  while (millis() - startTime < 3000) { 
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Cliente conectado, enviando credenciales...");
      printf("SSID: %s\n", userSSID);
      printf("Password: %s\n", userPassword);

      client.println(userSSID);
      client.println(userPassword);
      client.flush();
      delay(100);          
      client.stop();
      return true;  // Éxito
    }
    delay(100);
  }

  Serial.println("Timeout: ningún cliente se conectó.");
  return false;  // Fallo por timeout
}
