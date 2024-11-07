// Originally by Kris Winer, 2014, updated to work with ICM20948 by Brent Wilkins, 2016. Further updated by Charles Nicholson, 2024.

#include "AHRSAlgorithms.h"
#include "ICM20948.h"
#include "settings.h"

ICM20948 imu;

void setup() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);

  Wire.begin();
  // TWBR = 12;  // If needed, set I2C speed to be 400 kbit/sec.

  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB) {
    delay(10);
  }

  // Reset ICM20948.
  imu.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, READ_FLAG);
  delay(100);
  imu.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, 0x01);
  delay(100);

  byte c = imu.readByte(ICM20948_ADDRESS, WHO_AM_I_ICM20948);
  SerialUSB.print(F("ICM20948 I AM 0x"));
  SerialUSB.print(c, HEX);
  SerialUSB.print(F(" I should be 0x"));
  SerialUSB.println(0xEA, HEX);

  if (c == 0xEA) { // WHO_AM_I should always be 0x71.
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

    imu.calibrateICM20948(imu.gyroBias, imu.accelBias); // Calibrate gyro and accelerometers, load biases in bias registers.

    imu.initICM20948();
    SerialUSB.println("ICM20948 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of comms.
    byte d = imu.readByte(AK09916_ADDRESS, WHO_AM_I_AK09916);
    SerialUSB.print("AK8963 ");
    SerialUSB.print("I AM 0x");
    SerialUSB.print(d, HEX);
    SerialUSB.print(" I should be 0x");
    SerialUSB.println(0x09, HEX);

    if (d != 0x09) {
      SerialUSB.println(F("Communication with magnetometer failed, abort!"));
      SerialUSB.flush();
      abort();
    }

    imu.initAK09916(); // Get magnetometer calibration from AK8963 ROM.
    SerialUSB.println("AK09916 initialized for active data mode...");

    imu.getAres();
    imu.getGres();
    imu.getMres();

    // The next call delays for 4 seconds, and then records about 15 seconds of data to calculate bias and scale.
    SerialUSB.println("Keep the magnetometer still during this time... I think.");
    imu.magCalICM20948(imu.magBias, imu.magScale);
    SerialUSB.println("AK09916 mag biases (mG)");
    SerialUSB.println(imu.magBias[0]);
    SerialUSB.println(imu.magBias[1]);
    SerialUSB.println(imu.magBias[2]);

    SerialUSB.println("AK09916 mag scale (mG)");
    SerialUSB.println(imu.magScale[0]);
    SerialUSB.println(imu.magScale[1]);
    SerialUSB.println(imu.magScale[2]);
    delay(2000);
  } else { // If the WHO_AM_I read from the ICM20948 is incorrect, stop here.
    SerialUSB.print("Could not connect to ICM20948: 0x");
    SerialUSB.println(c, HEX);
    SerialUSB.println(F("Communication failed, abort!"));
    SerialUSB.flush();
    abort();
  }
  SerialUSB.print("Yaw, Pitch, Roll: ");
}

void loop() {
  if (imu.readByte(ICM20948_ADDRESS, INT_STATUS_1) & 0x01) {
    imu.readAccelData(imu.accelCount);
    imu.ax = (float)imu.accelCount[0] * imu.aRes;
    imu.ay = (float)imu.accelCount[1] * imu.aRes;
    imu.az = (float)imu.accelCount[2] * imu.aRes;

    imu.readGyroData(imu.gyroCount);
    imu.gx = (float)imu.gyroCount[0] * imu.gRes;
    imu.gy = (float)imu.gyroCount[1] * imu.gRes;
    imu.gz = (float)imu.gyroCount[2] * imu.gRes;

    imu.readMagData(imu.magCount);
    imu.mx = (float)imu.magCount[0] * imu.mRes - imu.magBias[0];
    imu.my = (float)imu.magCount[1] * imu.mRes - imu.magBias[1];
    imu.mz = (float)imu.magCount[2] * imu.mRes - imu.magBias[2];
  }

  imu.updateTime();
  MahonyQuaternionUpdate(imu.ax, imu.ay, imu.az, imu.gx * DEG_TO_RAD, imu.gy * DEG_TO_RAD, imu.gz * DEG_TO_RAD, imu.my, imu.mx, imu.mz, imu.deltat);

  imu.delt_t = millis() - imu.count;
  if (imu.delt_t > PRINT_FREQUENCY) {
    imu.yaw = atan2(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() * *(getQ() + 3)), *getQ() * *getQ() + *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) - *(getQ() + 3) * *(getQ() + 3));
    imu.pitch = -asin(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() * *(getQ() + 2)));
    imu.roll = atan2(2.0f * (*getQ() * *(getQ() + 1) + *(getQ() + 2) * *(getQ() + 3)), *getQ() * *getQ() - *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) + *(getQ() + 3) * *(getQ() + 3));

    imu.pitch *= RAD_TO_DEG;
    imu.yaw *= RAD_TO_DEG;

    imu.yaw -= DECLINATION;
    imu.roll *= RAD_TO_DEG;

    SerialUSB.print(imu.yaw, 2);
    SerialUSB.print(", ");
    SerialUSB.print(imu.pitch, 2);
    SerialUSB.print(", ");
    SerialUSB.println(imu.roll, 2);

    imu.count = millis();
  }
}
