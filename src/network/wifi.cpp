#include "wifi.h"

const char* ap_ssid = "Lockity_camera_config";
const char* ap_password = "&E43+8kpG'sTbFq2)zw3RnLG2jqOKYrk:{#iLe]U6'+`Z*&@SG";

WiFiManager wm;

bool shouldSaveConfig = false;
bool has_saved_credentials = false;
uint8_t wifi_timeout = 60;

// Solo configura el modo AP + STA y el AP
void setup_ap() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
}

// Callback llamado cuando se guardan nuevas credenciales
void saveConfigCallback() {
  Serial.println("Guardar configuración WiFi...");
  shouldSaveConfig = true;
}

void reset_credentials() {
  wm.resetSettings();
}

bool wifi_connect() {
  wm.setSaveConfigCallback(saveConfigCallback);
  wm.setConfigPortalTimeout(60);

  // Inicia portal config si no conecta a WiFi previamente configurado
  if (!wm.autoConnect("Lockity_config")) {
    Serial.println("❌ No se pudo conectar a router.");
    return false;
  }

  Serial.println("✅ STA conectada a internet.");
  return true;
}

void display_wifi_status(WifiState state) {
    switch (state) {
        case WIFI_IDLE:
            display_draw_center_text("Waiting...", 30);
            break;
        case WIFI_CONNECTING:
            display_draw_center_text("Connecting to WiFi...", 30);
            break;
        case WIFI_CONNECTED:
            display_draw_center_text("✅ WiFi connected!\nInitializing system", 30);
            break;
        case WIFI_FAILED:
            display_draw_center_text("❌ Connection failed", 30);
            break;
        case WIFI_TIMEOUT:
            display_draw_center_text("⏱ Timeout or canceled", 30);
            break;
    }
}
