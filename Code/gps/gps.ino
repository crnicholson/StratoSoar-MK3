#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define GPS_RX_PIN 10
#define GPS_TX_PIN 11

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("GPS Example");
  ss.begin(4800);
  delay(1000);

//   ss.print("$PCAS11,5*18\r\n"); // Set the dynamic model to be airborne with <1g acceleration.
//   Serial.print(ss.read(), HEX);
//   delay(1000);
}

void loop() {
  if (gps.location.isValid()) {
    Serial.print(F("Latitude: "));
    Serial.println(gps.location.lat(), 6);
    Serial.print(F("Longitude: "));
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println(F("Waiting for valid GPS signal..."));
  }
  delay(1000);
}