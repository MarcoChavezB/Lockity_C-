#pragma once

#include <U8g2lib.h>
#include <Wire.h>
#include <network/model.h>
#include <utils/icons.h>
#include <system.h>

void display_init();
void display_clear();
void display_logo();
void display_draw_center_text(const String &text, int startY, int lineHeight = 12);
void display_home();
