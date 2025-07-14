#include <U8g2lib.h>
#include <Wire.h>
#include <network/model.h>
#include <utils/icons.h>

void display_init();
void display_clear();
void display_drawString(int x, int y, const char* str, bool connected, bool camera_connected);
void display_drawHome(bool connected, bool camera_connected);
void display_test();
void display_logo();