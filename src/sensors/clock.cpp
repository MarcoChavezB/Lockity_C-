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

    Ds1302::DateTime now;
    rtc.getDateTime(&now);
    bool relojMal = (now.year < 24 || now.year > 40 || now.month == 0 || now.month > 12);
    
    if (relojMal || rtc.isHalted()) {
        Ds1302::DateTime dt = {
            .year = 25,  // 2025
            .month = Ds1302::MONTH_JUL,
            .day = 31,
            .hour = 16,
            .minute = 50,
            .second = 0,
            .dow = Ds1302::DOW_THU
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


bool is_within_schedule() {
  Ds1302::DateTime now;
  rtc.getDateTime(&now);

  int currentTime = now.hour * 3600 + now.minute * 60 + now.second;

  String currentDay = "";
  switch (now.dow) {
    case Ds1302::DOW_SUN: currentDay = "sun"; break;
    case Ds1302::DOW_MON: currentDay = "mon"; break;
    case Ds1302::DOW_TUE: currentDay = "tue"; break;
    case Ds1302::DOW_WED: currentDay = "wed"; break;
    case Ds1302::DOW_THU: currentDay = "thu"; break;
    case Ds1302::DOW_FRI: currentDay = "fri"; break;
    case Ds1302::DOW_SAT: currentDay = "sat"; break;
  }

  String todayStr = "20" + String(now.year) + "-";
  todayStr += (now.month < 10 ? "0" : "") + String(now.month) + "-";
  todayStr += (now.day < 10 ? "0" : "") + String(now.day);

  for (int i = 0; i < scheduleCount; i++) {
    Schedule s = schedules[i];

    bool isMatch = false;

    if (s.repeat_schedule) {
      if (s.day_of_week == currentDay) {
        isMatch = true;
      }
    } else {
      if (s.schedule_date.length() >= 10 && s.schedule_date.substring(0, 10) == todayStr) {
        isMatch = true;
      }
    }

    if (!isMatch) continue;

    int sh = s.start_time.substring(0, 2).toInt();
    int sm = s.start_time.substring(3, 5).toInt();
    int ss = s.start_time.substring(6, 8).toInt();
    int startSeconds = sh * 3600 + sm * 60 + ss;

    int eh = s.end_time.substring(0, 2).toInt();
    int em = s.end_time.substring(3, 5).toInt();
    int es = s.end_time.substring(6, 8).toInt();
    int endSeconds = eh * 3600 + em * 60 + es;

    if (currentTime >= startSeconds && currentTime <= endSeconds) {
      return true;
    }
  }
  return false;
}

