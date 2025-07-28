#include "camera.h"

void take_picture(){
    mqtt_publish((serial_number + "/command/picture").c_str(), "{value: 1}");
}