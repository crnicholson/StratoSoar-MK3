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
#include <LoRa.h>
#include <SPI.h>

struct receive {
  float lat;
  float lon;
  short volts;
  short alt;
  byte speed;
  byte seconds;
  byte minutes;
  byte hours;
  short txCount;
  char text[7];
} receivedData;

int rxCount;
long snr;
long rssi;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

#ifdef DEVMODE
  Serial.begin(BAUD_RATE);
  while (!Serial)
    longBlink(LED);
  Serial.println("StratoSoar MK3 telemetry receiver.");
#endif

  LoRa.setPins(SS_PIN, RESET_PIN, DIO0_PIN); // Has to be before LoRa.begin().

  if (!LoRa.begin(FREQUENCY)) {
#ifdef DEVMODE
    Serial.println("Starting LoRa failed!");
#endif
    while (1)
      longBlink(LED);
  }

  LoRa.setSyncWord(SYNC_WORD);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.crc(); // Checksum.

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
    float voltage = receivedData.volts / 100;
    rxCount++;

    // Check if the packet is a valid packet.
    if (sizeof(receivedData) == packetSize) {
#ifdef DEVMODE
      displayData();
#endif
#ifndef DEVMODE
      rssi = LoRa.packetRssi();
      snr = LoRa.packetSnr();

      Serial.write((uint8_t *)&receivedData.lat, sizeof(float)); // Send data over serial to the Python SondeHub uploader.
      Serial.write((uint8_t *)&receivedData.lng, sizeof(float));
      Serial.write((uint8_t *)&receivedData.alt, sizeof(long));
      Serial.write((uint8_t *)&receivedData.speed, sizeof(long));
      Serial.write((uint8_t *)&receivedData.txCnt, sizeof(long));
      Serial.write((uint8_t *)&rssi, sizeof(long));
      Serial.write((uint8_t *)&snr, sizeof(long));
#endif
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