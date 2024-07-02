#pragma once

#include "settings.h"
#include <Arduino.h>
#include <TinyGPSPlus.h>

extern long sleepStart;
extern bool sleepStarted;

extern TinyGPSPlus gps;

void gpsSetup();
void displayInfo();
void gpsSleepTime(long);
void gpsSleep();
void gpsWakeup(bool = true);