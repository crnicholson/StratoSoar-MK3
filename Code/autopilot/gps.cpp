#pragma once

long sleepStart;
bool sleepStarted;

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);

void gpsSetup() {
#ifdef DEVMODE
  SerialUSB.println("ATGM336H GPS Testing Code");
#endif
  ss.begin(9600);
  delay(1000);
#ifdef DEVMODE
  SerialUSB.println("Setting dynamic model to airborne.");
#endif
  ss.print("$PCAS11,5*18\r\n"); // Set the dynamic model to be airborne with <1g acceleration.
  delay(1000);
}

void displayInfo() {
  SerialUSB.print(F("Location: "));
  if (gps.location.isValid()) {
    SerialUSB.print(gps.location.lat(), 6);
    SerialUSB.print(F(","));
    SerialUSB.print(gps.location.lng(), 6);
  } else {
    SerialUSB.print(F("INVALID"));
  }

  SerialUSB.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    SerialUSB.print(gps.date.month());
    SerialUSB.print(F("/"));
    SerialUSB.print(gps.date.day());
    SerialUSB.print(F("/"));
    SerialUSB.print(gps.date.year());
  } else {
    SerialUSB.print(F("INVALID"));
  }

  SerialUSB.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10)
      SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.hour());
    SerialUSB.print(F(":"));
    if (gps.time.minute() < 10)
      SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.minute());
    SerialUSB.print(F(":"));
    if (gps.time.second() < 10)
      SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.second());
    SerialUSB.print(F("."));
    if (gps.time.centisecond() < 10)
      SerialUSB.print(F("0"));
    SerialUSB.print(gps.time.centisecond());
  } else {
    SerialUSB.print(F("INVALID"));
  }

  SerialUSB.println();
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

void gpsWakeup(bool waitForFix) { // Default is to wait for a fix, as defined in gps.h.
  digitalWrite(GPS_SLEEP_PIN, HIGH);
  if (waitForFix) {
    while (!gps.location.isValid()) { // Wait for a valid location before continuing with sketch.
      return 0;
    }
  }
}
