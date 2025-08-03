#include "wifi.h"

const char* ap_ssid = "Lockity_camera_config";
const char* ap_password = "&E43+8kpG'sTbFq2)zw3RnLG2jqOKYrk:{#iLe]U6'+`Z*&@SG";

WiFiManager wm;
WiFiServer server(3333);

bool shouldSaveConfig = false;
bool has_saved_credentials = false;
uint8_t wifi_timeout = 60;


void setup_ap() {
  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("🔧 Access Point creado en IP: ");
  Serial.println(IP);
}

// Callback llamado cuando se guardan nuevas credenciales
void saveConfigCallback() {
  Serial.println("Guardar configuración WiFi...");
  shouldSaveConfig = true;
}

void reset_credentials(){
    wm.resetSettings();
}

bool wifi_connect() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
  //websocket_setup();
  wm.setSaveConfigCallback(saveConfigCallback);
  wm.setConfigPortalTimeout(60);
  
  display_wifi_status(WIFI_CONNECTING);

  if (!wm.autoConnect("Lockity_config")) {
    Serial.println("❌ Timeout o cancelación en portal WiFi. Continuando sin conexión.");
    display_wifi_status(WIFI_TIMEOUT);
    delay(1000);
    return false;
  }

  Serial.println("Conectado a WiFi!");
  display_wifi_status(WIFI_CONNECTED);
  delay(1000);
  has_saved_credentials = wm.getWiFiIsSaved();
  
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
