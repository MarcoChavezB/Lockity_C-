#include <Arduino.h>
#include <system.h>
#include <network/wifi.h>
#include <network/websockets/websocket.h> 
#include <WebServer.h>

WebServer server(80); 


unsigned long last_check = 0;
const unsigned long check_interval = 30000;

void setup() {
  Serial.begin(115200);
  setup_ap();

  Serial.println("🔒 Starting Lockity Locker...");

  // Configuración básica de hardware y pantalla
  init_system();

  display_home();
  delay(5000);
  
  display_draw_center_text("Initializing internet...", 30);
  
  delay(1000);
  
  display_draw_center_text("Connect to\nLockity_config\nand go to 192.168.4.1", 20);
  
  // Conexión WiFi
    if (connect_wifi()) {
        if (!fetch_locker_config()) {
            if (exist_initial_config()) {
                display_draw_center_text("Using values\nfrom memory", 20);
                has_initial_config = true;
                File file = SPIFFS.open("/config.json", FILE_READ);
                parse_initial_config(file.readString());
                delay(2000);
            } else {
                display_draw_center_text("No config found\nSet up locker in system", 20);
                has_initial_config = false;
                delay(200);
            }
        }
        display_draw_center_text("Config loaded", 20);
        delay(2000);

        if (!get_schedules()) {
            Serial.println("❌ No se pudieron obtener los horarios de apertura del locker");
        }
        setup_network_services();
        
        Serial.println("ajustando servos");
        setup_servo();
        
        server.begin();
    } else {
        File file = SPIFFS.open("/config.json", FILE_READ);
        parse_initial_config(file.readString());
        setup_servo(); // también en el caso sin WiFi
    }

  
  //reset_credentials();

  display_draw_center_text("WELCOME \n TO \n LOCKITY", 20);
  delay(2000);
  print_config_to_serial();
  listFiles();
}


void loop() {
    server.handleClient();
    fingerprint_loop();
    display_home();
    websocket_loop();
    check_drawer_timeout();
    
    if(has_wifi){
        mqtt_loop(); 
    }
    
     // Reintento periódico de conexión si se perdió
    if (millis() - last_check > check_interval) {
        last_check = millis();

        if (!WiFi.isConnected() && has_saved_credentials) {
            Serial.println("🔄 Reintentando conexión WiFi...");
            has_wifi = wifi_connect();
        }

        // Si recuperó Internet, inicializa servicios de red
        if (has_wifi && !already_mqtt_setup) {
            Serial.println("🌐 Conexión a Internet restaurada. Configurando MQTT...");
            mqtt_setup();
        }
    }
}


/*void setup() {
  Serial.begin(115200);

  setup_ap();

  if(wifi_connect()) {
    Serial.println("Conectado a internet");
  }

  websocket_setup();
  server.begin();
}

void loop() {
  server.handleClient();
  websocket_loop();
}*/
