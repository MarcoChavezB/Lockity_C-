#include "model.h"

// Datos globales del locker
Usuario usuarios[3];
int usuarioCount = 0;
String serial_number = "87654321";
String idLocker = "";
bool toggleTopic = false;
Topic topics[10];
int topicsCount = 0;

// variables globales para loggin
String pendingAction = "";
int pendingUserId = -1;
int pendingCompartmentNumber = -1;
bool logReady = false;
