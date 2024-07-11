/*
i2c_scan.cpp, part of StratoSoar MK3, for an autonomous glider.
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

#include "i2c_scan.h"

void i2cScan() {
  int nDevices = 0;

#ifdef DEVMODE
  SerialUSB.println("Scanning...");
#endif

  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
#ifdef DEVMODE
      SerialUSB.print("I2C device found at address 0x");
      if (address < 16) {
        SerialUSB.print("0");
      }
      SerialUSB.print(address, HEX);
      SerialUSB.println("!");
#endif
      ++nDevices;
    } else if (error == 4) {
#ifdef DEVMODE
      SerialUSB.print("Unknown error at address 0x");
      if (address < 16) {
        SerialUSB.print("0");
      }
      SerialUSB.println(address, HEX);
#endif
    }
  }
  // #if (!defined(TEST_COORD) && !defined(USE_EEPROM)) || (defined(TEST_COORD) && defined(USE_EEPROM))
  //   if (nDevices >= 1) {
  // #ifdef DEVMODE
  //     SerialUSB.println("All I2C devices found successfully.");
  // #endif
  //   } else {
  // #ifdef DEVMODE
  //     SerialUSB.println("One I2C device is not responding.");
  // #endif
  //     longPulse(ERR_LED);
  //   }
  // #endif
  // #if !defined(TEST_COORD) && defined(USE_EEPROM)
  //   if (nDevices == 2) {
  // #ifdef DEVMODE
  //     SerialUSB.println("All I2C devices found successfully.");
  // #endif
  //   } else {
  // #ifdef DEVMODE
  //     SerialUSB.println("One or more I2C devices are not responding.");
  // #endif
  //     longPulse(ERR_LED);
  //   }
  // #endif
  // #if defined(TEST_COORD) && !defined(USE_EEPROM) && defined(DEVMODE)
  //   SerialUSB.println("Whether or not any devices were found, no I2C devices are needed.");
  // #endif
}