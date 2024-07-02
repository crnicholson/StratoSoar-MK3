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