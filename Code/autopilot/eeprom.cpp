#include "eeprom.cpp"

int eepromAddress;

void eepromSetup() {

  writeTime = (FLIGHT_TIME * 60) / (eepromSize / BYTES_PER_CYCLE) + EEPROM_BUFFER;
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

writeByteToEEPROM(int address, byte value) {
  if (sizeof(byte(value)) == 1) {
    eeprom.write(eepromAddress, byte(valye));
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