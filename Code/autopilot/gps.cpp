/*
gps.cpp, part of StratoSoar MK3, for an autonomous glider.
Copyright (C) 2024 Charles Nicholson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "gps.h"

TinyGPSPlus gps;

long sleepStart;
bool sleepStarted;

void gpsSetup() {
  Serial.begin(GPS_BAUD_RATE);
  delay(1000);

#ifdef DEVMODE
  SerialUSB.println("Setting GPS dynamic model to airborne.");
#endif

  Serial.print("$PCAS11,5*18\r\n"); // Set the dynamic model to be airborne with <1g acceleration.
  delay(1000);

#ifdef DEVMODE
  SerialUSB.println("Testing GPS conbection for 5 seconds.");
#endif

  long start = millis();
  while (millis() - start < 5000) {
    while (Serial.available() > 0) {
      gps.encode(Serial.read());
    }
    if (millis() > 3000 && gps.charsProcessed() < 7) {
#ifdef DEVMODE
      SerialUSB.println(F("No GPS detected: check wiring."));
#endif
      while (1)
        longBlink(ERR_LED);
      ;
    }
  }

#ifdef DEVMODE
  SerialUSB.println("GPS connected correctly.");
#endif
}

void getGPSData() {
  if (!Serial.available() == 0) {
    while (Serial.available() > 0) {
      gps.encode(Serial.read());
    }

    if (gps.location.isValid() && gps.altitude.isValid() && gps.time.isValid() && gps.date.isValid()) {
      lat = gps.location.lat();
      lon = gps.location.lng();
      altitude = gps.altitude.meters();
      year = gps.date.year();
      month = gps.date.month();
      day = gps.date.day();
      hour = gps.time.hour();
      minute = gps.time.minute();
      second = gps.time.second();
    }
  }
}

void getGPSLocation() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.location.isValid() && gps.altitude.isValid()) {
    lat = gps.location.lat();
    lon = gps.location.lng();
    altitude = gps.altitude.meters();
  }
}

void getGPSTime() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.time.isValid()) {
    hour = gps.time.hour();
    minute = gps.time.minute();
    second = gps.time.second();
  }
}

void getGPSDate() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.date.isValid()) {
    month = gps.date.month();
    day = gps.date.day();
    year = gps.date.year();
  }
}

void gpsSleepTime(long ms) {
  if (!sleepStarted) {
    sleepStart = millis();
    gpsSleep();
    sleepStarted = true;
  } else if (millis() - sleepStart >= ms) {
    gpsWakeup();
    sleepStarted = false;
  }
}

void gpsSleep() {
  digitalWrite(GPS_SLEEP_PIN, LOW);
}

void gpsWakeup(bool wait) { // Default is to wait for a fix, as defined in gps.h.
  digitalWrite(GPS_SLEEP_PIN, HIGH);
  if (wait) {
    waitForFix();
  }
}

void waitForFix() {
  while (!gps.location.isValid()) {
    while (Serial.available() > 0) {
      gps.encode(Serial.read());
    }
#ifdef DEVMODE
    SerialUSB.println("Waiting for a GPS fix.");
    delay(980);
#endif
    delay(20);
  }
}