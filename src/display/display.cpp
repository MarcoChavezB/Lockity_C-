#include "display.h"
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void display_init() {
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);
    u8g2.setBitmapMode(1);
    u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a font
    u8g2.sendBuffer();
}

void display_clear() {
    u8g2.clearBuffer();
}

void display_drawString(int x, int y, const char* str, bool connected, bool camera_connected) {
    display_clear();
    int lineHeight = u8g2.getMaxCharHeight();
    
    char line[64];
    int lineIndex = 0;
    int currentY = y;

    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n' || lineIndex >= sizeof(line) - 1) {
            line[lineIndex] = '\0';
            u8g2.drawStr(x, currentY, line);
            currentY += lineHeight;
            lineIndex = 0;
        } else {
            line[lineIndex++] = str[i];
        }
    }

    if (lineIndex > 0) {
        line[lineIndex] = '\0';
        u8g2.drawStr(x, currentY, line);
    }
    
    u8g2.drawStr(2, 7, "Lockity");
    if(connected){
        u8g2.drawXBM(111, 0, 15, 16, con_icon);
    } else{
        u8g2.drawXBM(111, 0, 15, 16, un_con_icon);
    }
    
    if(camera_connected){
        u8g2.drawXBM(110, 18, 17, 16, camera_icon);
    } else{
        u8g2.drawXBM(108, 27, 9, 8, alert_icon);
        u8g2.drawXBM(110, 18, 17, 16, camera_icon);
    }

    u8g2.sendBuffer();
}


void display_logo(){
    u8g2.clearBuffer();
    u8g2.drawXBM(40, 1, 16, 19, wifi_logo);
    u8g2.drawXBM(4, 7, 40, 48, logo);
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(80, 38, "Lockity");
    u8g2.sendBuffer();
}

void display_test(){
    u8g2.clearBuffer();
    u8g2.drawStr(2, 7, "Lockity");
    u8g2.drawXBM(111, 0, 15, 16, un_con_icon);
    u8g2.drawXBM(110, 20, 15, 16, con_icon);
    u8g2.sendBuffer();
}

void display_drawHome(bool connected, bool camera_connected) {
  u8g2.clearBuffer();
  u8g2.drawStr(2, 7, "Lockity");
  
  if(connected){
    u8g2.drawXBM(111, 0, 15, 16, con_icon);
  } else{
    u8g2.drawXBM(111, 0, 15, 16, un_con_icon);
  }
  
      
    if(camera_connected){
        u8g2.drawXBM(110, 18, 17, 16, camera_icon);
    } else{
        u8g2.drawXBM(108, 27, 9, 8, alert_icon);
        u8g2.drawXBM(110, 18, 17, 16, camera_icon);
    }

  
  int boxWidth = 15;
  int boxHeight = 10;
  int boxX = 0;
  int boxYSpacing = 5;
  int marginTop = 18; 

  u8g2.setFont(u8g2_font_6x10_tf);

  for (int i = 0; i < usuarioCount; i++) {
    int y = marginTop + i * (boxHeight + boxYSpacing);
    u8g2.drawFrame(boxX, y, boxWidth, boxHeight);
    u8g2.drawStr(boxX + boxWidth + 4, y + 11, usuarios[i].name.c_str());
  }

  u8g2.sendBuffer();
}

