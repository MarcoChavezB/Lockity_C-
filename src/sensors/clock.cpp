#include "clock.h"

// Pines para el DS1302
#define PIN_ENA 19  // RST
#define PIN_CLK 18  // CLK
#define PIN_DAT 23  // DAT


// Inicializar el RTC
Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);

const static char* WeekDays[] = {
  "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday", "Sunday"
};

void clock_setup(){
    rtc.init();
    
    if (rtc.isHalted()) {
    Serial.println("RTC detenido. Configurando hora...");

    Ds1302::DateTime dt = {
    .year = 25,
    .month = Ds1302::MONTH_JUL,
    .day = 25,
    .hour = 16,
    .minute = 30,
    .second = 0,
    .dow = Ds1302::DOW_WED
    };


    rtc.setDateTime(&dt);
  }
}

String get_time(){
  Ds1302::DateTime now;
  rtc.getDateTime(&now);

  static uint8_t last_second = 0;
  if (last_second != now.second) {
    last_second = now.second;

    Serial.printf("20%02d-%02d-%02d %s %02d:%02d:%02d\n",
      now.year,
      now.month,
      now.day,
      WeekDays[now.dow - 1],
      now.hour,
      now.minute,
      now.second
    );
  }
  return String("20") + String(now.year) + "-" +
         String(now.month) + "-" + String(now.day) + " " +
         WeekDays[now.dow - 1] + " " +
         String(now.hour) + ":" + String(now.minute) + ":" + String(now.second);
         
}

String get_time_stamp(){
    Ds1302::DateTime now;
    rtc.getDateTime(&now);
    
    return String("20") + String(now.year) + "-" +
             String(now.month) + "-" + String(now.day) + " " +
             String(now.hour) + ":" + String(now.minute) + ":" + String(now.second);
}