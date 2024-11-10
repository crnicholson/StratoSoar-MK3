/*
gps.ino, part of StratoSoar MK3, for an autonomous glider.
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

#include <TinyGPSPlus.h>

// Settings.
#define BAUD_RATE 115200 // Baud rate for serial monitor.
#define GPS_BAUD 9600    // Baud rate for GPS module.

#define SLEEP // Test the sleep mode of the GPS module.

#define SLEEP_PIN 12 // Sleep pin for GPS module, to be connected to EN or ON/OFF.

int status = 2; // 0 = no GPS, 1 = GPS, 2 = unknown.
long sleepStart, connectionTestStart;
bool sleepStarted;

TinyGPSPlus gps;

void setup() {
  pinMode(SLEEP_PIN, OUTPUT);
  digitalWrite(SLEEP_PIN, LOW);

  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB)
    ;
  delay(1000);
  SerialUSB.println("ATGM336H GPS testing code.");
  SerialUSB.println("Checking for GPS module connection.");
  Serial1.begin(GPS_BAUD);
  delay(100);

  connectionTestStart = millis();

  while (millis() - connectionTestStart < 5000) {
    while (Serial1.available() > 0)
      gps.encode(Serial1.read());

    if (millis() - connectionTestStart > 3000 && gps.charsProcessed() < 7) {
      SerialUSB.println("No GPS detected: check wiring. Freezing sketch.");
      while (1)
        ;
    }
  }

  SerialUSB.println("GPS module connected.");

  SerialUSB.println("Setting dynamic model to airborne.");
  Serial1.print("$PCAS11,5*18\r\n"); // Set the dynamic model to be airborne with <1g acceleration.
  delay(1000);

#ifdef SLEEP
  SerialUSB.println("Testing GPS sleep mode for 10 seconds.");
  gpsSleep();
  delay(10000);
  gpsWakeup(false); // Wake up, but don't wait for fix.
  SerialUSB.println("Done sleeping GPS, hopefully power consumption went down.");
#endif
}

void loop() {
  while (Serial1.available() > 0)
    if (gps.encode(Serial1.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    SerialUSB.println("No GPS detected: check wiring. Freezing sketch.");
    while (true)
      ;
  }

  // gpsSleepTime(60000) // Sleep for 60 seconds.
}

void displayInfo() {
  SerialUSB.print("Location: ");
  if (gps.location.isValid()) {
    SerialUSB.print(gps.location.lat(), 6);
    SerialUSB.print(",");
    SerialUSB.print(gps.location.lng(), 6);
  } else {
    SerialUSB.print("Invalid");
  }

  SerialUSB.print(" Date: ");
  if (gps.date.isValid()) {
    SerialUSB.print(gps.date.month());
    SerialUSB.print("/");
    SerialUSB.print(gps.date.day());
    SerialUSB.print("/");
    SerialUSB.print(gps.date.year());
  } else {
    SerialUSB.print("Invalid");
  }

  SerialUSB.print(" Time: ");
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10)
      SerialUSB.print("0");
    SerialUSB.print(gps.time.hour());
    SerialUSB.print(":");
    if (gps.time.minute() < 10)
      SerialUSB.print("0");
    SerialUSB.print(gps.time.minute());
    SerialUSB.print(":");
    if (gps.time.second() < 10)
      SerialUSB.print("0");
    SerialUSB.print(gps.time.second());
    SerialUSB.print(".");
    if (gps.time.centisecond() < 10)
      SerialUSB.print("0");
    SerialUSB.print(gps.time.centisecond());
  } else {
    SerialUSB.print("Invalid");
  }

  SerialUSB.println();
}

void gpsSleepTime(long ms) {
  if (!sleepStarted) {
    sleepStart = millis();
    gpsSleep();
    sleepStarted = true;
  } else if (millis() - sleepStart >= ms) {
    gpsWakeup(true); // Wait for a fix after waking up GPS.
    sleepStarted = false;
  }
}

// Sleep GPS until woken up with gpsWakeup().
void gpsSleep() {
  digitalWrite(SLEEP_PIN, HIGH);
}

void gpsWakeup(bool waitForFix) {
  digitalWrite(SLEEP_PIN, LOW);
  if (waitForFix) {
    while (!gps.location.isValid() && !gps.altitude.isValid() && !gps.time.isValid() && !gps.date.isValid()) // Wait for a valid data before continuing with sketch.
      gps.encode(Serial1.read());
  }
}
