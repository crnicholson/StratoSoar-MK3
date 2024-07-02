#include "gps.h"

bool sleepStarted;
long sleepStart;

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

float getGPSLocation() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.location.isValid()) {
    return gps.location.lat(), gps.location.lng(), gps.altitude.meterrs();
  } else {
    return 0;
  }
}

float getGPSTime() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.time.isValid()) {
    return gps.time.hour(), gps.time.minute(), gps.time.second();
  } else {
    return 0;
  }
}

float getGPSDate() {
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
#ifdef DEVMODE
    SerialUSB.println("Waiting for a GPS fix.");
    delay(950);
#endif
    delay(50);
  }
}