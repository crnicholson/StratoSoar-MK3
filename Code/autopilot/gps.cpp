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

long sleepStart, connectionTestStart;
bool sleepStarted;

void gpsSetup() {
  Serial1.begin(GPS_BAUD_RATE);
  delay(1000);

#ifdef DEVMODE
  SerialUSB.println("Testing GPS connection for 5 seconds.");
#endif

  connectionTestStart = millis();

  while (millis() - connectionTestStart < 5000) {
    while (Serial1.available() > 0)
      gps.encode(Serial1.read());

    if (millis() - connectionTestStart > 3000 && gps.charsProcessed() < 7) {
#ifdef DEVMODE
      SerialUSB.println("No GPS detected: check wiring. Freezing sketch.");
#endif
      while (1)
        longBlink(ERR_LED);
    }
  }

#ifdef DEVMODE
  SerialUSB.println("GPS connected correctly.");
#endif

#ifdef DEVMODE
  SerialUSB.println("Now setting GPS dynamic model to airborne.");
#endif

  Serial1.print("$PCAS11,5*18\r\n"); // Set the dynamic model to be airborne with <1g acceleration.
  delay(1000);
}

void getGPSData() {
  if (!Serial1.available() == 0) {
    while (Serial1.available() > 0) {
      gps.encode(Serial1.read());
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
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

  if (gps.location.isValid() && gps.altitude.isValid()) {
    lat = gps.location.lat();
    lon = gps.location.lng();
    altitude = gps.altitude.meters();
  }
}

void getGPSTime() {
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

  if (gps.time.isValid()) {
    hour = gps.time.hour();
    minute = gps.time.minute();
    second = gps.time.second();
  }
}

void getGPSDate() {
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
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
  while (!gps.location.isValid() || !gps.time.isValid() || !gps.date.isValid()) {
    while (Serial1.available() > 0) {
      gps.encode(Serial1.read());
    }
#ifdef DEVMODE
    SerialUSB.println("Waiting for a GPS fix.");
    SerialUSB.print("Current HDOP: ");
    SerialUSB.println(gps.hdop.hdop());
#endif
    delay(1000);
  }
}