#include "display.h"
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
int yPositions[] = {16, 37, 58};      
int iconYPositions[] = {4, 25, 46};

extern volatile uint8_t attemp_fingerprint;

void display_init() {
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);
    u8g2.setBitmapMode(1);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();
}

void display_clear() {
    u8g2.clearBuffer();
}

void display_logo(){
    u8g2.clearBuffer();
    u8g2.drawXBM(40, 1, 16, 19, wifi_logo);
    u8g2.drawXBM(4, 7, 40, 48, logo);
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(80, 38, "Lockity");
    u8g2.sendBuffer();
}

void display_draw_center_text(const String &text, int startY, int lineHeight) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB08_tr);

    int y = startY;
    int lineStart = 0;

    for (int i = 0; i <= text.length(); i++) {
        if (text[i] == '\n' || i == text.length()) {
            String line = text.substring(lineStart, i);
            int textWidth = u8g2.getStrWidth(line.c_str());
            int x = (u8g2.getDisplayWidth() - textWidth) / 2;
            u8g2.drawStr(x, y, line.c_str());
            y += lineHeight;
            lineStart = i + 1;
        }
    }

    u8g2.sendBuffer();
}

void display_home() {

    static unsigned long fingerprintDisplayTimestamp = 0;
    static bool showFingerprintResult = false;

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.drawFrame(0, 1, 97, 21);
    u8g2.drawFrame(0, 22, 97, 21);

    int displayIndex = 0;

    for (int i = 0; i < usuarioCount; i++) {
      if (usuarios[i].role == "user") {
        if (displayIndex >= usuarioUsersCount) break;  // Limita a la cantidad de usuarios 'user'

        // Dibuja icono y nombre
        u8g2.drawXBM(2, iconYPositions[displayIndex], 18, 18, drawer_draw);
        u8g2.drawStr(24, yPositions[displayIndex], usuarios[i].name.c_str());

        // Dibuja número del primer cajón asignado (si tiene alguno)
        if (usuarios[i].cajon_count > 0) {
          String drawerNumStr = usuarios[i].cajones_usuario[0];
          u8g2.drawStr(84, yPositions[displayIndex], drawerNumStr.c_str());
        }

        bool anyDrawerOpen = false;

        for (int j = 0; j < usuarios[i].cajon_count; j++) {
          int drawerNum = atoi(usuarios[i].cajones_usuario[j].c_str());
          int drawerIndex = getDrawerIndexByNumber(drawerNum);
          if (drawerIndex != -1) {
            if (drawers[drawerIndex].state == DRAWER_OPEN) {
              anyDrawerOpen = true;
              break;
            }
          }
        }

        if (anyDrawerOpen) {
          u8g2.drawXBM(5, iconYPositions[displayIndex], 13, 13, circle_draw);
        }

        displayIndex++;
      }
    }

    u8g2.drawFrame(0, 43, 97, 21);

    if(has_wifi){
      u8g2.drawXBM(103, 3, 19, 16, wifi_icon);
    } else{
      u8g2.drawXBM(103, 3, 19, 16, wifi_icon_no_connected);
    }

    u8g2.drawXBM(104, 24, 17, 16, camera_icon);

    if(!camera_connected){
      u8g2.drawXBM(115, 29, 10, 16, cross_icon);
    }

    // Mostrar icono de resultado huella durante 1 seg
    if (attemp_fingerprint != 0) {
      fingerprintDisplayTimestamp = millis();
      showFingerprintResult = true;
    }

    if (showFingerprintResult) {
      if (millis() - fingerprintDisplayTimestamp < 1000) {
        if(attemp_fingerprint == 1){
          u8g2.drawXBM(108, 45, 10, 9, success_icon);
        }
        else if(attemp_fingerprint == 2){
          u8g2.drawXBM(103, 42, 18, 18, error_icon);
        }
      } else {
        // Después de 1 segundo, ocultar icono y resetear estado
        showFingerprintResult = false;
        attemp_fingerprint = 0;
      }
    } else {
      u8g2.drawXBM(104, 43, 18, 18, drawer_draw);
    }

    u8g2.setFont(u8g2_font_6x13B_tr);
    u8g2.sendBuffer();
}

// ============== función para mostrar el dashboard ==============
void display_loop(){
  // Aquí puedes agregar actualizaciones periódicas si necesitas
}
