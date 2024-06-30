#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

#define GPS_RX_PIN 10
#define GPS_TX_PIN 11
#define SLEEP_PIN 12

long sleepStart;
bool sleepStarted;

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);

void setup() {
  pinMode(SLEEP_PIN, OUTPUT);
  digitalWrite(SLEEP_PIN, HIGH);

  Serial.begin(115200);
  Serial.println("ATGM336H GPS Testing Code");
  ss.begin(9600);
  delay(1000);

  Serial.println("Setting dynamic model to airborne.");
  ss.print("$PCAS11,5*18\r\n"); // Set the dynamic model to be airborne with <1g acceleration.
  delay(1000);

  Serial.println("Testing GPS sleep mode for 10 seconds.");
  gpsSleep();
  delay(10000);
  gpsWakeup(waitForFix = false);
  Serial.println("Done sleeping GPS, hopefully h")
}

void loop() {
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      ;
  }
  
  // gpsSleepTime(60000) // Sleep for 60 seconds.
}

void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
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
  digitalWrite(SLEEP_PIN, LOW);
}

void gpsWakeup(bool waitForFix) { // Default is to wait for a fix, as defined in gps.h.
  digitalWrite(SLEEP_PIN, HIGH);
  if (waitForFix) {
    while (!gps.location.isValid()) { // Wait for a valid location before continuing with sketch.
      return 0;
    }
  }
}
