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

int rxCount;
long snr;
long rssi;
float newTLat, newTLon;

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

void loop() {
  int packetSize = LoRa.parsePacket(); // Parse packet.
  if (packetSize > 0) {
    shortBlink(LED);
    LoRa.readBytes((byte *)&receivedData, sizeof(receivedData)); // Receive packet and put it into a struct.
    rxCount++;

    // Check if the packet is a valid packet.
    if (sizeof(receivedData) == packetSize) {
      shortBlink(LED);
#ifdef DEVMODE
      displayData();
#endif
#ifndef DEVMODE
      rssi = LoRa.packetRssi();
      snr = LoRa.packetSnr();

      Serial.write((uint8_t *)&receivedData.lat, sizeof(float)); // Send data over serial to the Python SondeHub uploader.
      Serial.write((uint8_t *)&receivedData.lon, sizeof(float));
      Serial.write((uint8_t *)&receivedData.altitude, sizeof(float));
      Serial.write((uint8_t *)&receivedData.tLat, sizeof(float));
      Serial.write((uint8_t *)&receivedData.tLon, sizeof(float));
      Serial.write((uint8_t *)&receivedData.temperature, sizeof(float));
      Serial.write((uint8_t *)&receivedData.pressure, sizeof(float));
      Serial.write((uint8_t *)&receivedData.humidity, sizeof(long));
      Serial.write((uint8_t *)&receivedData.volts, sizeof(float));
      Serial.write((uint8_t *)&receivedData.yaw, sizeof(long));
      Serial.write((uint8_t *)&receivedData.pitch, sizeof(long));
      Serial.write((uint8_t *)&receivedData.roll, sizeof(long));
      Serial.write((uint8_t *)&receivedData.hour, sizeof(long));
      Serial.write((uint8_t *)&receivedData.minute, sizeof(long));
      Serial.write((uint8_t *)&receivedData.second, sizeof(long));
      Serial.write((uint8_t *)&receivedData.abort, sizeof(long));
      Serial.write((uint8_t *)&receivedData.txCount, sizeof(long));
      Serial.write((uint8_t *)&rxCount, sizeof(long));
      Serial.write((uint8_t *)&U_LAT, sizeof(float));
      Serial.write((uint8_t *)&U_LON, sizeof(float));
      Serial.write((uint8_t *)&U_ALT, sizeof(float));
      Serial.write((uint8_t *)&rssi, sizeof(long));
      Serial.write((uint8_t *)&snr, sizeof(long));
#endif
    }
  }
  if (Serial.available() > 0) {
    Serial.read((byte *)&newTLat, sizeof(float));
    Serial.read((byte *)&newTLon, sizeof(float));
    if newTLon
      != 0 && newTLat != 0 && newTLon != receivedData.tLon &&newTLat != receivedData.tLat {
        LoRa.beginPacket();
        LoRa.write((byte *)&newTLat, sizeof(float));
        LoRa.write((byte *)&newTLon, sizeof(float));
        LoRa.endPacket(true); // Send in async mode.
      }
  }
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