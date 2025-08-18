#pragma once
#include <HTTPClient.h>

#define MAX_CAJONES 5
#define MAX_DRAWERS 3
#define MAX_SCHEDULES 10
#define MAX_COMPONENTS 15
#define MAX_PINS 5

struct URLRoutes {
  String initial_config;
  String public_logs;
  String get_schedules;
  String change_drawer_state;
  String still_open_alert;
};

enum DrawerState {
  DRAWER_CLOSED,
  DRAWER_OPEN,
  DRAWER_MAINTENANCE,
  DRAWER_ERROR
};

struct Drawer {
  int number;
  DrawerState state;
};


extern Drawer drawers[MAX_DRAWERS];

struct Topic {
  String key;
  String value;
};

struct Schedule{
    uint8_t id;
    String day_of_week;
    String start_time;
    String end_time;
    bool repeat_schedule;
    String schedule_date;
};

struct Usuario {
  String id_usuario;
  String name;
  String role;
  String cajones_usuario[MAX_CAJONES];
  int cajon_count;
};

extern Usuario usuarios[MAX_CAJONES];
extern int usuarioCount;
extern int usuarioUsersCount;
extern String idLocker;
extern String serial_number;
extern bool toggleTopic;
extern Topic topics[10];
extern int topicsCount;

extern String pendingAction;
extern int pendingUserId;
extern int pendingCompartmentNumber;
extern bool logReady;


// variables globales de horarios
extern Schedule schedules[MAX_SCHEDULES];
extern int scheduleCount;



// --------------------- NUEVO PARA COMPONENTES ---------------------

struct PinInfo {
  int id;
  int componentId;
  String pinName;
  int pinNumber;
};

struct Component {
  int id;
  String type;
  String model;
  String status;
  PinInfo pins[MAX_PINS];
  int pinCount;
};

extern Component components[MAX_COMPONENTS];
extern int componentCount;

// ---------------------------------------------------------------


const char* drawerStateToString(DrawerState state);
int getPinsByType(const String& type, int* pinBuffer);
int getDrawerIndexByNumber(int drawerNumber);
