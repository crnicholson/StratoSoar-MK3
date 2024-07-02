#include "settings.h"

#include "gps.h"
#include "i2c_scan.h"

float lat, lon;

void setup() {
  pinMode(GPS_SLEEP_PIN, OUTPUT);
  digitalWrite(GPS_SLEEP_PIN, HIGH);

#ifdef DEVMODE
  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB)
    ;
  SerialUSB.println("StratoSoar MK3.0 Autopilot.");
#endif

#ifdef USE_GPS
  gpsSetup();
#endif
#ifndef USE_GPS
  lat = 41, lon = -71;
#endif
}

void loop() {
#ifdef USE_GPS
  lat, lon = getGPSData();
#endif
}
