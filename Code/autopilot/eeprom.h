#pragma once

#include "settings.h"
#include <Arduino.h>
#include <SparkFun_External_EEPROM.h> // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM

void eepromSetup();
void writeDataToEEPROM(float lat, float lon, int altitude, short yaw, short pitch, short roll, byte hour, byte minute, byte second);
void writeByteToEEPROM(int address, byte value);
void writeFloatToEEPROM(int address, float value);
void writeIntToEEPROM(int address, int value);
void writeShortToEEPROM(int address, short value);