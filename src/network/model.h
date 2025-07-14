#pragma once
#include <HTTPClient.h>

#define MAX_CAJONES 3

struct URLRoutes {
  String initial_config;
  String status_update;
};

struct Topic {
  String key;
  String value;
};


struct Usuario {
  String id_usuario;
  String name;
  String cajones_usuario[MAX_CAJONES];
  int cajon_count;
};

extern Usuario usuarios[3];
extern int usuarioCount;
extern String idLocker;
extern String serial_number;
extern bool toggleTopic;
extern Topic topics[10];
extern int topicsCount;