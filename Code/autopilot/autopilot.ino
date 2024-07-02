#include "settings.h"

#include "gps.h"
#include "i2c_scan.h"

float lat, lon, altitude, 
int year, month, day, hour, minute, second;

void setup() {
  pinMode(GPS_SLEEP_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ERR_LED, OUTPUT);
  pinMode(VOLTMETER_PIN, INPUT);
  digitalWrite(GPS_SLEEP_PIN, HIGH); // GPS on for default.
  digitalWrite(LED, LOW);
  digitalWrite(ERR_LED, LOW);

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
  lat, lon = getGPSLocation();
#endif
}
