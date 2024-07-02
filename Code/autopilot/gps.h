#pragma once

#include "blink.h"
#include "settings.h"
#include <Arduino.h>
#include <TinyGPSPlus.h>

extern long sleepStart;
extern bool sleepStarted;

extern TinyGPSPlus gps;

void gpsSetup();
float getGPSData();
void displayInfo();
void gpsSleepTime(long ms);
void gpsSleep();
void gpsWakeup(bool wait = true);
void waitForFix()