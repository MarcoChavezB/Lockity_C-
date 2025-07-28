#pragma once
#include <HTTPClient.h>
#include "model.h"
#include "storage/spiffs.h"
#include "sensors/clock.h"
bool get_initial_config();
bool publish_toggle_log(String base64Pic, int userId, int compartment_number, String action);