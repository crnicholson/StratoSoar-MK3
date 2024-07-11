/*
imu.cpp, part of StratoSoar MK3, for an autonomous glider.
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

#include "imu.h"

ICM20948 imu;

void imuSetup() {
  // Reset ICM20948.
  imu.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, READ_FLAG);
  delay(100);
  imu.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, 0x01);
  delay(100);

  byte c = imu.readByte(ICM20948_ADDRESS, WHO_AM_I_ICM20948);
#ifdef DEVMODE
  SerialUSB.print(F("ICM20948 I AM 0x"));
  SerialUSB.print(c, HEX);
  SerialUSB.print(F(" I should be 0x"));
  SerialUSB.println(0xEA, HEX);
#endif

  if (c == 0xEA) { // WHO_AM_I should always be 0x71.
#ifdef DEVMODE
    SerialUSB.println(F("ICM20948 is online..."));

    // Start by performing self test and reporting values.
    imu.ICM20948SelfTest(imu.selfTest);
    SerialUSB.print(F("x-axis self test: acceleration trim within : "));
    SerialUSB.print(imu.selfTest[0], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("y-axis self test: acceleration trim within : "));
    SerialUSB.print(imu.selfTest[1], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("z-axis self test: acceleration trim within : "));
    SerialUSB.print(imu.selfTest[2], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("x-axis self test: gyration trim within : "));
    SerialUSB.print(imu.selfTest[3], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("y-axis self test: gyration trim within : "));
    SerialUSB.print(imu.selfTest[4], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("z-axis self test: gyration trim within : "));
    SerialUSB.print(imu.selfTest[5], 1);
    SerialUSB.println("% of factory value");
#endif

    imu.calibrateICM20948(imu.gyroBias, imu.accelBias); // Calibrate gyro and accelerometers, load biases in bias registers.

    imu.initICM20948();
#ifdef DEVMODE
    SerialUSB.println("ICM20948 initialized for active data mode....");
#endif

    // Read the WHO_AM_I register of the magnetometer, this is a good test of comms.
    byte d = imu.readByte(AK09916_ADDRESS, WHO_AM_I_AK09916);
#ifdef DEVMODE
    SerialUSB.print("AK8963 ");
    SerialUSB.print("I AM 0x");
    SerialUSB.print(d, HEX);
    SerialUSB.print(" I should be 0x");
    SerialUSB.println(0x09, HEX);
#endif

    if (d != 0x09) {
#ifdef DEVMODE
      SerialUSB.println(F("Communication with magnetometer failed, abort!"));
      SerialUSB.flush();
#endif
      abort();
    }

    imu.initAK09916(); // Get magnetometer calibration from AK8963 ROM.
#ifdef DEVMODE
    SerialUSB.println("AK09916 initialized for active data mode...");
#endif

    imu.getAres();
    imu.getGres();
    imu.getMres();

// The next call delays for 4 seconds, and then records about 15 seconds of data to calculate bias and scale.
#ifdef DEVMODE
    SerialUSB.println("Keep the magnetometer still during this time... I think.");
#endif
    imu.magCalICM20948(imu.magBias, imu.magScale);
#ifdef DEVMODE
    SerialUSB.println("AK09916 mag biases (mG)");
    SerialUSB.println(imu.magBias[0]);
    SerialUSB.println(imu.magBias[1]);
    SerialUSB.println(imu.magBias[2]);

    SerialUSB.println("AK09916 mag scale (mG)");
    SerialUSB.println(imu.magScale[0]);
    SerialUSB.println(imu.magScale[1]);
    SerialUSB.println(imu.magScale[2]);
#endif
    delay(2000);
  } else { // If the WHO_AM_I read from the ICM20948 is incorrect, stop here.
#ifdef DEVMODE
    SerialUSB.print("Could not connect to ICM20948: 0x");
    SerialUSB.println(c, HEX);
    SerialUSB.println(F("Communication failed, abort!"));
    SerialUSB.flush();
#endif
    abort();
  }
}