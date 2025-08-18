#include "model.h"

// Datos globales del locker
Usuario usuarios[MAX_CAJONES];
int usuarioCount = 0;
int usuarioUsersCount = 0;
String serial_number = "SN-2025-0745-AX93-PLQ7";
// LK-2025-8438-EF75-UP5S
// SN-2025-0745-AX93-PLQ7 // loker princila
String idLocker = "";
bool toggleTopic = false;
Topic topics[10];
int topicsCount = 0;

Component components[MAX_COMPONENTS];
int componentCount = 0;


// variables globales para loggin
String pendingAction = "";
int pendingUserId = -1;
int pendingCompartmentNumber = -1;
bool logReady = false;

// variables para hoarios de apertura en locker
Schedule schedules[MAX_SCHEDULES];
int scheduleCount = 0;
//SN-2025-0745-AX93-PLQ7


Drawer drawers[MAX_DRAWERS] = {
  {1, DRAWER_CLOSED},
  {2, DRAWER_CLOSED},
  {3, DRAWER_CLOSED},
};

const char* drawerStateToString(DrawerState state) {
  switch (state) {
    case DRAWER_CLOSED: return "CLOSED";
    case DRAWER_OPEN: return "OPEN";
    case DRAWER_MAINTENANCE: return "MAINTENANCE";
    case DRAWER_ERROR: return "ERROR";
    default: return "UNKNOWN";
  }
}


int getDrawerIndexByNumber(int drawerNumber) {
  for (int i = 0; i < MAX_DRAWERS; i++) {
    if (drawers[i].number == drawerNumber) {
      return i; 
    }
  }
  return -1;  
}


// Devuelve la cantidad de pines encontrados y llena el arreglo pinBuffer
int getPinsByType(const String& type, int* pinBuffer) {
    int count = 0;
    for (int i = 0; i < componentCount; i++) {
        if (components[i].type == type) {
            for (int j = 0; j < components[i].pinCount; j++) {
                pinBuffer[count] = components[i].pins[j].pinNumber;
                count++;
            }
        }
    }
    return count;
}
