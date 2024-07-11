/*
eeprom.cpp, part of StratoSoar MK3, for an autonomous glider.
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

#include "eeprom.h"

int eepromAddress, startTimer;

ExternalEEPROM eeprom; // Initialize EEPROM.

void eepromSetup() {
#ifdef USE_EEPROM
  eeprom.setMemoryType(EEPROM_SIZE); // Valid types: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1025, 2048

  if (eeprom.begin() == false) {
#ifdef DEVMODE
    SerialUSB.println("No EEPROM detected. Freezing sketch.");
#endif
    while (1) {
      longPulse(ERR_LED, 0);
    }
  }

  eepromSize = eeprom.length();
#ifdef DEVMODE
  SerialUSB.println("EEPROM detected!");
  SerialUSB.print("EEPROM size in bytes: ");
  SerialUSB.println(eepromSize);
#endif
#ifdef ERASE_EEPROM
#ifdef DEVMODE
  SerialUSB.println("Erasing EEPROM, should take ~10 seconds.");
#endif
  startTimer = millis();
  eeprom.erase();
#ifdef DEVMODE
  SerialUSB.print("Done erasing. Time took in milliseconds: ");
  SerialUSB.println(mills() - startTimer);
#endif
#endif

  eeprom.write(0, 200); // Testing EEPROM.

#ifdef DEVMODE
  SerialUSB.println("Testing EEPROM.");
  SerialUSB.print("EEPROM reads (should be 200): ");
  SerialUSB.println(eeprom.read(0));
#endif
  if (eeprom.read(0) != 200) {
#ifdef DEVMODE
    SerialUSB.println("Warning: EEPROM is not working as expected. Try removing the average write time finder from the code.");
#endif
    longPulse(ERR_LED, 0);
  }
#endif

  //   writeTime = (FLIGHT_TIME * 60) / (eepromSize / BYTES_PER_CYCLE) + EEPROM_BUFFER;
}

void writeDataToEEPROM(float lat, float lon, short altitude, short yaw, short pitch, short roll, byte hour, byte minute, byte second) {
  writeFloatToEEPROM(eepromAddress, lat);
  writeFloatToEEPROM(eepromAddress, lon);
  writeShortToEEPROM(eepromAddress, altitude);
  writeShortToEEPROM(eepromAddress, yaw);
  writeShortToEEPROM(eepromAddress, pitch);
  writeShortToEEPROM(eepromAddress, roll);
  writeByteToEEPROM(eepromAddress, hour);
  writeByteToEEPROM(eepromAddress, minute);
  writeByteToEEPROM(eepromAddress, second);
}

void writeByteToEEPROM(int address, byte value) {
  if (sizeof(byte(value)) == 1) {
    eeprom.write(eepromAddress, byte(value));
  } else {
    eeprom.write(eepromAddress, 255); // Write 255 to the EEPROM if the value is too large.
  }
  eepromAddress++;
  while (eeprom.isBusy()) {
    delayMicroseconds(100);
  }
}

void writeFloatToEEPROM(int address, float value) {
  byte *p = (byte *)(void *)&value;
  for (int i = 0; i < sizeof(value); i++) {
    eeprom.write(address + i, *p++);
  }
  for (int i = sizeof(value); i < 4; i++) {
    eeprom.write(address + i, 0);
  }
  eepromAddress += 4;
  while (eeprom.isBusy()) {
    delayMicroseconds(100);
  }
}

void writeIntToEEPROM(int address, int value) {
  byte *p = (byte *)(void *)&value;
  int bytesWritten = 0;
  for (int i = 0; i < sizeof(value); i++, bytesWritten++) {
    eeprom.write(address + i, *p++);
  }
  for (int i = bytesWritten; i < 4; i++) {
    eeprom.write(address + i, 0);
  }
  eepromAddress += 4;
  while (eeprom.isBusy()) {
    delayMicroseconds(100);
  }
}

void writeShortToEEPROM(int address, short value) {
  byte *p = (byte *)(void *)&value;
  for (int i = 0; i < sizeof(value); i++) {
    eeprom.write(address + i, *p++);
  }
  eepromAddress += 2;
  while (eeprom.isBusy()) {
    delayMicroseconds(100);
  }
}