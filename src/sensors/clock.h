#pragma once
#include <Arduino.h>
#include <Ds1302.h>
#include "network/model.h"

void clock_setup();
String get_time();
String get_time_stamp();
unsigned long get_current_seconds();
bool is_within_schedule();
