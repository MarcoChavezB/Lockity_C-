#pragma once
#include <Arduino.h>
#include <Ds1302.h>

void clock_setup();
String get_time();
String get_time_stamp();