#include <system.h>

bool has_wifi = false;
bool already_mqtt_setup = false;
bool camera_connected = false;
bool has_initial_config = false;
volatile uint8_t attemp_fingerprint = 0;

void init_system() {
  Serial.println("inciando sistema");
  clock_setup();
  display_init();
  display_logo();
  delay(2000);

  //setup_servo();
  setup_alarm();

  if(!fingerprint_init()){
    Serial.println("Error al inicial el fingerprint");
  } 

  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Error al montar SPIFFS");
    ESP.restart();
  }
  
  //reset_credentials();
  reset_system_data();
}

void reset_system_data(){
  Serial.println("🔄 Reiniciando datos del sistema...");
  fingerprint_delete_all();
  delete_config();
  //reset_credentials();
  Serial.println("✅ Datos del sistema reiniciados.");
}


bool connect_wifi() {
  has_wifi = wifi_connect();
  return has_wifi;
}


bool fetch_locker_config() {
    int attempts = 0;

    while (!get_initial_config() && attempts < 5) {
        Serial.println("Waiting for system configuration...");
        display_draw_center_text("Waiting for\nsystem config...", 20);
        delay(3000);
        attempts++;
    }

    if (attempts >= 5) {
        display_draw_center_text("Failed to\nfetch config", 20);
        delay(2000);
        return false;
    }

    display_draw_center_text("System config\nreceived!", 20);
    delay(1500);

    print_config_to_serial();
    return true;
}

void setup_network_services() {
  mqtt_setup();
  websocket_setup();
  already_mqtt_setup = true;
}


bool check_internet_access() {
  WiFiClient client;
  client.setTimeout(3000);
  return client.connect("1.1.1.1", 80); 
}


void listFiles() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
    Serial.print("Archivo: ");
    Serial.print(file.name());
    Serial.print("\tTamaño: ");
    Serial.println(file.size());
    file = root.openNextFile();
  }
}
