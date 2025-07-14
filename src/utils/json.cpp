#include "json.h"

bool json_parser(const String &doc, JsonDocument &outDoc) {
  DeserializationError error = deserializeJson(outDoc, doc);

  if (error) {
    Serial.print("Error al parsear JSON: ");
    Serial.println(error.c_str());
    return false;
  }

  return true;
}
