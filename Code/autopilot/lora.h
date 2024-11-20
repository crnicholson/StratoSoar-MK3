#pragma once

#include "blink.h"
#include "settings.h"
#include "vars.h"
#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

void loraSetup();
void sendData(struct data packet);
void sendHammingData(struct data packet);
byte hammingEncode(byte data);
void hammingReceive();
byte hammingDecode(byte data);