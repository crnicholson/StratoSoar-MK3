#ifndef GPS_H
#define GPS_H

#include "settings.h"
#include <Arduino.h>
#include <TinyGPSPlus.h>

void gpsSetup();
void displayInfo();
void gpsSleepTime(long);
void gpsSleep();
void gpsWakeup(bool = true);

#endif