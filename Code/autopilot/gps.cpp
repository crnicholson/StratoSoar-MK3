#include "gps.h"

long sleepStart;
bool sleepStarted;

TinyGPSPlus gps;

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

float getGPSData() {
  if (!Serial.available() == 0) {
    while (Serial.available() > 0) {
      gps.encode(Serial.read());
    }

    if (gps.location.isValid() && gps.altitude.isValid() && gps.time.isValid() && gps.date.isValid()) {
      return gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second();
    } else {
      return lat, lon, alt, year, month, day, hour, minute, second; // Return the previous values if the GPS is not valid.
    }
  } else {
    return lat, lon, alt, year, month, day, hour, minute, second; // Return the previous values if there is no data available.s
  }
}

float getGPSLocation() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.location.isValid() && gps.altitude.isValid()) {
    return gps.location.lat(), gps.location.lng(), gps.altitude.meters();
  } else {
    return 0;
  }
}

int getGPSTime() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.time.isValid()) {
    return gps.time.hour(), gps.time.minute(), gps.time.second();
  } else {
    return 0;
  }
}

int getGPSDate() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.date.isValid()) {
    return gps.date.month(), gps.date.day(), gps.date.year();
  } else {
    return 0;
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