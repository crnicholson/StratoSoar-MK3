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

#include "lora.h"
#include "misc.h"
#include "sd.h"
#include "settings.h"
#include "vars.h"
#include "websockets.h"

bool dataValid, newPacketForGlider;
long rxCount;

struct fromGlider fromGliderStruct;
struct toGlider toGliderStruct;

TaskHandle_t loop2;

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
  websocketsSetup(); // Set up the Websockets.
#endif

  xTaskCreatePinnedToCore(
      pollWebsockets,   // Task function.
      "pollWebsockets", // name of task.
      10000,            // Stack size of task.
      NULL,             // Parameter of the task.
      1,                // Priority of the task.
      &loop2,           // Task handle to keep track of created task.
      1);               // Pin task to core 1.
}

void loop() {
#ifdef HAMMING
  hammingReceive();
  if (dataValid) {
    shortBlink(LED);
    rssi = LoRa.packetRssi();
    snr = LoRa.packetSnr();
#ifdef WIFI
    sendToServer();
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
    shortPulse(LED);
    rssi = LoRa.packetRssi();
    snr = LoRa.packetSnr();
#ifdef WIFI
    sendToServer();
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

// This is using FreeRTOS to poll the Websockets.
void pollWebsockets(void *pvParameters) {
  if (client.available()) {
    client.poll(); // If there is data, a call back will be run in websockets.cpp. It will add all new data to toGliderStruct.
  }
  if (newPacketForGlider) {
    newPacketForGlider = false;
#ifdef HAMMING
    hammingSend();
#endif
#ifndef HAMMING
    normalSend();
#endif
  }
}