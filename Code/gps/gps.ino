#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define GPS_RX_PIN 10
#define GPS_TX_PIN 11

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  delay(1000);

  ss.print("$PCAS11,5*18\r\n"); // Set the dynamic model to be airborne with <1g acceleration.
  Serial.print(ss.read());
  delay(1000);
}

void loop() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.location.isValid()) {
        double latitude = gps.location.lat();
        double longitude = gps.location.lng();
        double altitude = gps.altitude.meters();

        Serial.print("Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);
        Serial.print("Altitude: ");
        Serial.println(altitude, 2);
      }
    }
  }
}