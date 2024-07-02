#include "settings.h"

#include "gps.h"

void setup() {
  pinMode(GPS_SLEEP_PIN, OUTPUT);
  digitalWrite(GPS_SLEEP_PIN, HIGH);

#ifdef DEVMODE
  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB)
    ;
  SerialUSB.println("StratoSoar MK3.0 Autopilot.");
#endif

  gpsSetup();
}

void loop() {
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    SerialUSB.println(F("No GPS detected: check wiring."));
    while (true)
      ;
  }
}
