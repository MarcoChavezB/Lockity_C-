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

unsigned long get_current_seconds() {
    Ds1302::DateTime now;
    rtc.getDateTime(&now);
    return now.hour * 3600UL + now.minute * 60UL + now.second;
}

void clock_setup() {
    rtc.init();

    Ds1302::DateTime now;
    rtc.getDateTime(&now);

        Serial.println("configurando relgo");

    if (rtc.isHalted() || now.year < 24 || now.year > 40 || now.month == 0 || now.month > 12) {
        // ⚠️ Solo la PRIMERA vez (o cuando cambies batería) debes ajustar la fecha real
        Ds1302::DateTime dt = {
            .year = 25,
            .month = Ds1302::MONTH_AUG,
            .day = 16,
            .hour = 10,
            .minute = 18,
            .second = 0,
            .dow = Ds1302::DOW_SAT
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
             String(now.month) + "-" + String(now.day) + "/" + String(now.hour) + ":" + String(now.minute) + ":" + String(now.second);
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

  Serial.println("📅 [is_within_schedule] --- Verificando horarios ---");
  Serial.print("Hora actual: ");
  Serial.print(now.hour); Serial.print(":");
  Serial.print(now.minute); Serial.print(":");
  Serial.println(now.second);

  Serial.print("Día actual (dow): "); Serial.println(currentDay);
  Serial.print("Fecha actual: "); Serial.println(todayStr);
  Serial.print("Segundos actuales: "); Serial.println(currentTime);

  for (int i = 0; i < scheduleCount; i++) {
    Schedule s = schedules[i];

    Serial.println("──────────────────────────────");
    Serial.print("Evaluando horario #"); Serial.println(i);
    Serial.print("Start: "); Serial.println(s.start_time);
    Serial.print("End:   "); Serial.println(s.end_time);
    Serial.print("Repeat: "); Serial.println(s.repeat_schedule ? "Sí" : "No");
    Serial.print("Day_of_week: "); Serial.println(s.day_of_week);
    Serial.print("Schedule_date: "); Serial.println(s.schedule_date);

    bool isMatch = false;

    if (s.repeat_schedule) {
      if (s.day_of_week == currentDay) {
        isMatch = true;
        Serial.println("✔ Coincide con el día de la semana");
      }
    } else {
      if (s.schedule_date.length() >= 10 && s.schedule_date.substring(0, 10) == todayStr) {
        isMatch = true;
        Serial.println("✔ Coincide con la fecha específica");
      }
    }

    if (!isMatch) {
      Serial.println("❌ No coincide ni día ni fecha, se omite.");
      continue;
    }

    int sh = s.start_time.substring(0, 2).toInt();
    int sm = s.start_time.substring(3, 5).toInt();
    int ss = s.start_time.substring(6, 8).toInt();
    int startSeconds = sh * 3600 + sm * 60 + ss;

    int eh = s.end_time.substring(0, 2).toInt();
    int em = s.end_time.substring(3, 5).toInt();
    int es = s.end_time.substring(6, 8).toInt();
    int endSeconds = eh * 3600 + em * 60 + es;

    Serial.print("Rango en segundos: ");
    Serial.print(startSeconds); Serial.print(" -> "); Serial.println(endSeconds);

    if (currentTime >= startSeconds && currentTime <= endSeconds) {
      Serial.println("✅ Está dentro del horario");
      return true;
    } else {
      Serial.println("⏱ Fuera del rango de tiempo");
    }
  }

  Serial.println("❌ Ningún horario válido encontrado");
  return false;
}

