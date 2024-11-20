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

#include "settings.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

bool dataValid;
long rxCount;

struct receive receiveStruct;

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

  loraSetup(); // Set up the LoRa.
  sdSetup();   // Set up the SD card.

#ifdef WIFI
  WiFi.begin(YOUR_SSID, YOUR_PASSWORD);
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
}

void loop() {
#ifdef HAMMING
  hammingReceive();
  if (dataValid) { 
    shortBlink(LED);
    rssi = LoRa.packetRssi();
    snr = LoRa.packetSnr();
#ifdef WIFI
    sendToSondehub();
#endif
    writeToCard();
#ifdef DEVMODE
    displayData();
  } else {
    Serial.println("Data is invalid.");
    #endif
  }
#endif
#ifndef HAMMING
  normalReceive();
  if (dataValid) {
    shortBlink(LED);
    rssi = LoRa.packetRssi();
    snr = LoRa.packetSnr();
#ifdef WIFI
    sendToSondehub();
#endif
    writeToCard();
#ifdef DEVMODE
    displayData();
  } else {
    Serial.println("Data is invalid.");
#endif
  }
#endif
}