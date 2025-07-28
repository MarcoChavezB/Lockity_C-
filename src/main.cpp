#include <Arduino.h>
#include <display/display.h>
#include <network/http.h>
#include <network/mqtt.h>
#include <network/wifi.h>
#include <sensors/fingerprint.h>
#include <storage/spiffs.h>
#include <motor/servo.h>
#include <sensors/clock.h>


bool connected_wifi = false;
bool cannectec_camera = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Lockity Locker...");
  clock_setup();
  // Elimina la red guardada
  //!reset_credentials();
  // Confguracion inicial del reloju

  
  // configuracion inicial de display
  display_init();
  
  display_logo();
  delay(2000);
  
  
  // setup de servomotores
  setup_servo();
  
  // setup de alarma
  setup_alarm();
  
  // configuracion inicial de fingerprint
  if (!fingerprint_init()) {
    display_drawString(0, 30, "Fingerprint error\n skiping...", connected_wifi, cannectec_camera);
    delay(3000);
  }
  
  fingerprint_delete_all();

  if (!SPIFFS.begin(true)) {
    return;
  }

  // Eliminar configuracion guardada
  delete_config();

  display_drawString(0, 30, "Connect to\nLockity_config red\nAnd go to 192.168.4.1", connected_wifi, cannectec_camera);
  connected_wifi = wifi_connect();
  
  while (!connected_wifi) {
    display_drawString(0, 30, "WIFI error.,\nRetrying...", connected_wifi, cannectec_camera);
    delay(3000);
  }
  display_drawString(0, 30, "Connected to WiFi", connected_wifi, cannectec_camera);
  
  delay(1000);
  
  display_drawString(0, 30, "Configuring camera... ", connected_wifi, cannectec_camera);

  if (!share_wifi()) {
    display_drawString(0, 30, "Camera not configured\n skiping...", connected_wifi, cannectec_camera);
    delay(3000);
  } else {
    cannectec_camera = true;
    display_drawString(0, 30, "Camera configured\n successfully...", connected_wifi, cannectec_camera);
  }

  // verificar si hay configuracion inicial del locker
  while (!get_initial_config()) {
    display_drawString(0, 30, "Error getting config\nRetrying...", connected_wifi, cannectec_camera);
    delay(3000);
  }

  display_drawString(0, 30, "Config found\n successfully...", connected_wifi, cannectec_camera);

  delay(1000);

  print_config_to_serial();

  // Mqtt configuration
  mqtt_setup();

  display_drawString(0, 30, "Loding data...", connected_wifi, cannectec_camera);
  delay(1000);

  display_drawHome(connected_wifi, cannectec_camera);
}

void loop() { 
    mqtt_loop(); 
    fingerprint_loop();
}
