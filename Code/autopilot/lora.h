#pragma once

#include "blink.h"
#include "settings.h"
#include "vars.h"
#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

void loraSetup();
void sendData(struct data &newPacket);
void sendHammingData(struct data &newPacket);
byte hammingEncode(byte data);
void hammingReceive();
byte hammingDecode(byte data);