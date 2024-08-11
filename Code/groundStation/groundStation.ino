/*
groundStation.ino, part of a receive for StratoSoar MK3, for an autonomous glider.
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

// To-do:
// - Make custom circuit board
// - Add support for ESP32 for BLE communication
// - Add GPS and compass to point towards the target - this could be done with an iPhone app
// - Add OLED to display glider position
// - Add bidirectoional communication for changing the landing location
// - Add way to abort the mission

// Note: This system operates at 433 mHz, so a ham radio license is needed in the US. Please check local regulations before use.

// LoRa API reference: https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md.

#include "settings.h"
#include <LoRa.h>
#include <SPI.h>

struct receive {
  float lat, lon, tLat, tLon, altitude, temperature, pressure, humidity, volts;
  short yaw, pitch, roll;
  byte hour, minute, second;
  short txCount;
  byte abort;
  char callSign[7] = CALL_SIGN;
} receivedData;

long rxCount;
byte abort;
long snr, rssi;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

#ifdef DEVMODE
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    longBlink(LED);
  }
  Serial.println("StratoSoar MK3 telemetry receiver.");
#endif

  LoRa.setPins(SS_PIN, RESET_PIN, DIO0_PIN); // Has to be before LoRa.begin().

  if (!LoRa.begin(FREQUENCY)) {
#ifdef DEVMODE
    Serial.println("Starting LoRa failed!");
#endif
    while (1) {
      longBlink(LED);
    }
  }

#ifndef FAST_LORA
  LoRa.setSyncWord(SYNC_WORD);               // Defined in settings.h.
  LoRa.setSpreadingFactor(SPREADING_FACTOR); // Defined in settings.h.
  LoRa.setSignalBandwidth(BANDWIDTH);        // Defined in settings.h.
  LoRa.crc();                                // Checksum for packet error detection.
#endif

#ifdef FAST_LORA
  LoRa.setSyncWord(SYNC_WORD); // Defined in settings.h.
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(250000);
  LoRa.crc(); // Checksum for packet error detection.
#endif

#ifdef DEVMODE
  Serial.println("LoRa initialized, starting in 1 second.");
#endif

  delay(1000);
}

shortBlink(LED);

void loop() {
  hammingRecieve();
}

void longBlink(int pin) {
  digitalWrite(pin, HIGH);
  delay(2000);
  digitalWrite(pin, LOW);
  delay(2000);
}

void shortBlink(int pin) {
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
  delay(100);
}

void displayData() {
  Serial.print("Lat: ");
  Serial.print(receivedData.lat, 6);

  Serial.print(" Lon: ");
  Serial.print(receivedData.lon, 6);

  Serial.print(" Alt: ");
  Serial.print(receivedData.altitude, 2);

  Serial.print(" Target lat: ");
  Serial.print(receivedData.tLat, 6);

  Serial.print(" Target lon: ");
  Serial.print(receivedData.tLon, 6);

  Serial.print(" Temp: ");
  Serial.print(receivedData.temperature, 2);

  Serial.print(" Pressure: ");
  Serial.print(receivedData.pressure, 2);

  Serial.print(" Humidity: ");
  Serial.print(receivedData.humidity);

  Serial.print(" Volts: ");
  Serial.print(receivedData.volts, 2);

  Serial.print(" Yaw: ");
  Serial.print(receivedData.yaw);

  Serial.print(" Pitch: ");
  Serial.print(receivedData.pitch);

  Serial.print(" Roll: ");
  Serial.print(receivedData.roll);

  Serial.print(" Time: ");
  Serial.print(receivedData.hour);
  Serial.print(":");
  Serial.print(receivedData.minute);
  Serial.print(":");
  Serial.print(receivedData.second);

  Serial.print(" TX count: ");
  Serial.print(receivedData.txCount);

  Serial.print(" RX count: ");
  Serial.print(rxCount);

  Serial.print(" Uploader lat: ");
  Serial.print(U_LAT, 6);

  Serial.print(" Uploader lon: ");
  Serial.print(U_LON, 6);

  Serial.print(" Uploader alt: ");
  Serial.print(U_ALT, 2);

  Serial.print(" RSSI: ");
  Serial.print(rssi);

  Serial.print(" SNR: ");
  Serial.println(snr);
}

void hammingRecieve() {
  int packetSize = LoRa.parsePacket(); // Parse packet.
  if (packetSize > 0) {
    shortBlink(LED);

    byte encodedData[2 * sizeof(receivedData)];
    LoRa.readBytes(encodedData, sizeof(encodedData));

    byte *decodedData = (byte *)&receivedData;

    for (size_t i = 0; i < sizeof(receivedData); ++i) {
      // Decode each byte using Hamming(7,4) code.
      byte highNibble = hammingDecode(encodedData[2 * i]);
      byte lowNibble = hammingDecode(encodedData[2 * i + 1]);

      decodedData[i] = (highNibble << 4) | lowNibble;
    }

    rxCount++;

    if (sizeof(receivedData) * 2 == packetSize) {
      shortBlink(LED);
      rssi = LoRa.packetRssi();
      snr = LoRa.packetSnr();
#ifdef DEVMODE
      displayData();
#endif
      if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        http.begin(client, serverName + "/add-data");
        http.addHeader("Content-Type", "application/json");

        JsonDocument doc;
        doc["lat"] = receivedData.lat;
        doc["lon"] = receivedData.lon;
        doc["altitude"] = receivedData.altitude;
        doc["tLat"] = receivedData.tLat;
        doc["tLon"] = receivedData.tLon;
        doc["temperature"] = receivedData.temperature;
        doc["pressure"] = receivedData.pressure;
        doc["humidity"] = receivedData.humidity;
        doc["volts"] = receivedData.volts;
        doc["yaw"] = receivedData.yaw;
        doc["pitch"] = receivedData.pitch;
        doc["roll"] = receivedData.roll;
        doc["hour"] = receivedData.hour;
        doc["minute"] = receivedData.minute;
        doc["second"] = receivedData.second;
        doc["abort"] = receivedData.abort;
        doc["txCount"] = receivedData.txCount;
        doc["rxCount"] = rxCount;
        doc["uLat"] = U_LAT; // Uploader location!
        doc["uLon"] = U_LON;
        doc["uAlt"] = U_ALT;
        doc["rssi"] = rssi;
        doc["snr"] = snr;
        doc["callsign"] = receivedData.callSign;

        String requestBody;
        serializeJson(doc, requestBody);

        int httpResponseCode = http.POST(requestBody);
        http.end(); // Free resources.

#ifdef DEVMODE
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
#endif

        WiFiClient client;
        HTTPClient http;

        http.begin(client, serverName + "/change-glider-data");
        http.addHeader("Content-Type", "application/json");

        JsonDocument doc;
        doc["callsign"] = receivedData.callSign;

        String requestBody;
        serializeJson(doc, requestBody);

        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("HTTP Response code: " + String(httpResponseCode));

          StaticJsonDocument responseDoc;
          DeserializationError error = deserializeJson(responseDoc, response);

          if (!error) {
            float newTLat = responseDoc["tLat"];
            float newTLon = responseDoc["tLon"];
            abort = byte(responseDoc["abort"]);

            // This makes sure that if abort is true, the packet will be sent. There's chance that it will not be sent if the lat and lon did not change.
            if (abort) {
              LoRa.beginPacket();
              LoRa.write((byte *)&newTLat, sizeof(float));
              LoRa.write((byte *)&newTLon, sizeof(float));
              LoRa.write(abort);
              LoRa.endPacket(true); // Send in async mode.
            } else {
              if (newTLon != receivedData.tLon && newTLat != receivedData.tLat) {
                LoRa.beginPacket();
                LoRa.write((byte *)&newTLat, sizeof(float));
                LoRa.write((byte *)&newTLon, sizeof(float));
                LoRa.write(abort);
                LoRa.endPacket(true); // Send in async mode.
              }
            }
          } else {
#ifdef DEVMODE
            Serial.println("Failed to parse JSON response");
#endif
          }
        } else {
#ifdef DEVMODE
          Serial.println("Error on HTTP request");
#endif
        }

        http.end(); // Free resources.

      } else {
#ifdef DEVMODE
        Serial.println("WiFi Disconnected.");
#endif
        while (1) {
          longBlink(LED);
        }
      }
    }
  }
}

// Function to decode Hamming(7,4) code and correct single-bit errors.
byte hammingDecode(byte encoded) {
  byte p1 = (encoded >> 6) & 0x1;
  byte p2 = (encoded >> 5) & 0x1;
  byte p3 = (encoded >> 4) & 0x1;
  byte d1 = (encoded >> 3) & 0x1;
  byte d2 = (encoded >> 2) & 0x1;
  byte d3 = (encoded >> 1) & 0x1;
  byte d4 = (encoded >> 0) & 0x1;

  // Calculate syndrome bits.
  byte s1 = p1 ^ d1 ^ d2 ^ d4;
  byte s2 = p2 ^ d1 ^ d3 ^ d4;
  byte s3 = p3 ^ d2 ^ d3 ^ d4;

  // Determine error position.
  byte errorPos = (s3 << 2) | (s2 << 1) | s1;

  // Correct the error if needed.
  if (errorPos > 0 && errorPos <= 7) {
    encoded ^= (1 << (7 - errorPos));
  }

  return (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
}