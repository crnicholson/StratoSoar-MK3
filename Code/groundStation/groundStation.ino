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

// Note: This system operates at 433 mHz, so a ham radio license is needed in the US. Please check local regulations before use.

// LoRa API reference: https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md.

#include "settings.h"
#include <ArduinoJson.h>
#include <LoRa.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>

struct receive {
  float lat, lon, tLat, tLon, altitude, temperature, pressure, humidity, volts, hdop;
  short yaw, pitch, roll;
  byte hour, minute, second;
  short txCount;
  byte abort;
  char callSign[7] = CALL_SIGN;
} receivedData;

long rxCount, snr, rssi;
bool abortFlight;
int packetSize;

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

#ifdef WIFI
  WiFi.begin(SSID, PASSWORD);
#ifdef DEVMODE
  Serial.println("Connecting to WiFi...");
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEVMODE
    Serial.print(".");
#endif
  }
#ifdef DEVMODE
  Serial.println("Connected to WiFi!");
#endif
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

#ifdef USE_SD
  // Initialize the SD card
  if (!SD.begin(chipSelect)) {
#ifdef DEVMODE
    Serial.println("SD card initialization failed!");
#endif
    while (1) {
      longBlink(LED);
    }
  }

#ifdef DEVMODE
  Serial.println("SD card is ready.");
#endif

  // Create or open the file.
  File dataFile = SD.open("data.csv", FILE_WRITE);

  // If the file opened successfully, write the header.
  if (dataFile) {
    dataFile.println("lat,lon,altitude,tLat,tLon,temperature,pressure,humidity,volts,yaw,pitch,roll,hour,minute,second,abort,txCount,rxCount,uLat,uLon,uAlt,rssi,snr,callsign");
    dataFile.close();
  } else {
#ifdef DEVMODE
    Serial.println("Error opening file for writing.");
#endif
    while (1) {
      longBlink(LED);
    }
  }
#endif

#ifdef DEVMODE
  Serial.println("LoRa initialized, starting in 1 second.");
#endif

  delay(1000);
}

void loop() {
#ifdef HAMMING
  hammingReceive();
  if (sizeof(receivedData) * 2 == packetSize) {
    shortBlink(LED);
    rssi = LoRa.packetRssi();
    snr = LoRa.packetSnr();
#ifdef WIFI
    sendToSondehub();
#endif
#ifdef USE_SD
    writeToCard();
#endif
#ifdef DEVMODE
    displayData();
#endif
  }
  e
#endif
#ifndef HAMMING
  normalReceive();
  if (sizeof(receivedData) == packetSize) {
    shortBlink(LED);
    rssi = LoRa.packetRssi();
    snr = LoRa.packetSnr();
#ifdef WIFI
    sendToSondehub();
#endif
#ifdef USE_SD
    writeToCard();
#endif
#ifdef DEVMODE
    displayData();
#endif
  }
#endif
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

void hammingReceive() {
  packetSize = LoRa.parsePacket(); // Parse packet.
  if (sizeof(receivedData) * 2 == packetSize) {
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
  }
}

void sendToSondehub() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName + "/add-data");
    http.addHeader("Content-Type", "application/json");

    // This makes a POST request to server that adds to SondeHub.
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
  } else {
#ifdef DEVMODE
    Serial.println("WiFi disconnected. Freezing sketch.");
#endif
    while (1)
      longBlink(LED);
  }
}

void sendToGlider() {
  if (WiFi.status() == WL_CONNECTED) {
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
        abortFlight = bool(responseDoc["abort"]);

        // This makes sure that if abort is true, the packet will be sent. There's a chance that it will not be sent if the lat and lon did not change.
        if (abort || (newTLon != receivedData.tLon && newTLat != receivedData.tLat)) {
          byte *dataToSend = new byte[9]; // 9 bytes: 4 for newTLat, 4 for newTLon, 1 for abort.
          memcpy(dataToSend, (byte *)&newTLat, sizeof(float));
          memcpy(dataToSend + sizeof(float), (byte *)&newTLon, sizeof(float));
          dataToSend[8] = abortFlight;

          byte encodedData[18]; // Each byte is encoded into 2 bytes

          for (size_t i = 0; i < 9; ++i) {
            encodedData[2 * i] = hammingEncode(dataToSend[i] >> 4);       // Encode high nibble
            encodedData[2 * i + 1] = hammingEncode(dataToSend[i] & 0x0F); // Encode low nibble
          }

          delete[] dataToSend; // Free the allocated memory

          // Send the encoded data
          LoRa.beginPacket();
          LoRa.write(encodedData, sizeof(encodedData));
          LoRa.endPacket(true); // Send in async mode.
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
    while (1)
      longBlink(LED);
  }
}

void normalReceive() {
  packetSize = LoRa.parsePacket();
  if (packetSize == sizeof(receivedData))
    LoRa.readBytes((byte *)&receivedData, sizeof(receivedData));
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

byte hammingEncode(byte data) {
  byte p1 = (data >> 2) & 0x1 ^ (data >> 1) & 0x1 ^ (data >> 0) & 0x1;
  byte p2 = (data >> 3) & 0x1 ^ (data >> 1) & 0x1 ^ (data >> 0) & 0x1;
  byte p3 = (data >> 3) & 0x1 ^ (data >> 2) & 0x1 ^ (data >> 0) & 0x1;

  byte hamming = (p1 << 6) | (p2 << 5) | (p3 << 4) | (data & 0xF);
  return hamming;
}

void writeToCard() {
  File dataFile = SD.open("data.csv", FILE_WRITE);

  if (dataFile) {
    dataFile.print(receivedData.lat);
    dataFile.print(",");
    dataFile.print(receivedData.lon);
    dataFile.print(",");
    dataFile.print(receivedData.altitude);
    dataFile.print(",");
    dataFile.print(receivedData.tLat);
    dataFile.print(",");
    dataFile.print(receivedData.tLon);
    dataFile.print(",");
    dataFile.print(receivedData.temperature);
    dataFile.print(",");
    dataFile.print(receivedData.pressure);
    dataFile.print(",");
    dataFile.print(receivedData.humidity);
    dataFile.print(",");
    dataFile.print(receivedData.volts);
    dataFile.print(",");
    dataFile.print(receivedData.yaw);
    dataFile.print(",");
    dataFile.print(receivedData.pitch);
    dataFile.print(",");
    dataFile.print(receivedData.roll);
    dataFile.print(",");
    dataFile.print(receivedData.hour);
    dataFile.print(",");
    dataFile.print(receivedData.minute);
    dataFile.print(",");
    dataFile.print(receivedData.second);
    dataFile.print(",");
    dataFile.print(receivedData.abort);
    dataFile.print(",");
    dataFile.print(receivedData.txCount);
    dataFile.print(",");
    dataFile.print(rxCount);
    dataFile.print(",");
    dataFile.print(U_LAT);
    dataFile.print(",");
    dataFile.print(U_LON);
    dataFile.print(",");
    dataFile.print(U_ALT);
    dataFile.print(",");
    dataFile.print(rssi);
    dataFile.print(",");
    dataFile.print(snr);
    dataFile.print(",");
    dataFile.println(receivedData.callSign);

    dataFile.close(); // Close the file.
    Serial.println("Data written to SD card.");
  } else {
    Serial.println("Error opening file for writing.");
  }
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