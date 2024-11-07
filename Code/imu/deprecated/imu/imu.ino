
// Originally by Kris Winer, 2014, updated to work with ICM20948 by Brent Wilkins, 2016. Further updated by Charles Nicholson, 2024.

#include "AHRSAlgorithms.h"
#include "ICM20948.h"
#include "settings.h"

#define AHRS true        // Set to false for basic data read
#define SerialDebug true // Set to true to get SerialUSB output for debugging

ICM20948 myIMU;

void setup() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);


  Wire.begin();
  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB) {
    delay(10);
  }

  // Reset ICM20948.
  myIMU.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, READ_FLAG);
  delay(100);
  myIMU.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, 0x01);
  delay(100);

  byte c = myIMU.readByte(ICM20948_ADDRESS, WHO_AM_I_ICM20948);
  SerialUSB.print(F("ICM20948 I AM 0x"));
  SerialUSB.print(c, HEX);
  SerialUSB.print(F(" I should be 0xEA"));
  SerialUSB.println();

  if (c == 0xEA) {
    SerialUSB.println(F("ICM20948 is online..."));

    myIMU.ICM20948SelfTest(myIMU.selfTest);
    SerialUSB.print(F("x-axis self test: acceleration trim within : "));
    SerialUSB.print(myIMU.selfTest[0], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("y-axis self test: acceleration trim within : "));
    SerialUSB.print(myIMU.selfTest[1], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("z-axis self test: acceleration trim within : "));
    SerialUSB.print(myIMU.selfTest[2], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("x-axis self test: gyration trim within : "));
    SerialUSB.print(myIMU.selfTest[3], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("y-axis self test: gyration trim within : "));
    SerialUSB.print(myIMU.selfTest[4], 1);
    SerialUSB.println("% of factory value");
    SerialUSB.print(F("z-axis self test: gyration trim within : "));
    SerialUSB.print(myIMU.selfTest[5], 1);
    SerialUSB.println("% of factory value");

    myIMU.calibrateICM20948(myIMU.gyroBias, myIMU.accelBias);
    myIMU.initICM20948();
    SerialUSB.println("ICM20948 initialized for active data mode....");

    byte d = myIMU.readByte(AK09916_ADDRESS, WHO_AM_I_AK09916);
    SerialUSB.print("AK8963 I AM 0x");
    SerialUSB.print(d, HEX);
    SerialUSB.print(" I should be 0x09");
    SerialUSB.println();

    if (d != 0x09) {
      SerialUSB.println(F("Communication with magnetometer failed, abort!"));
      SerialUSB.flush();
      abort();
    }

    myIMU.initAK09916();
    SerialUSB.println("AK09916 initialized for active data mode....");

    myIMU.getAres();
    myIMU.getGres();
    myIMU.getMres();
    myIMU.magCalICM20948(myIMU.magBias, myIMU.magScale);
    SerialUSB.println("AK09916 mag biases (mG)");
    SerialUSB.println(myIMU.magBias[0]);
    SerialUSB.println(myIMU.magBias[1]);
    SerialUSB.println(myIMU.magBias[2]);

    SerialUSB.println("AK09916 mag scale (mG)");
    SerialUSB.println(myIMU.magScale[0]);
    SerialUSB.println(myIMU.magScale[1]);
    SerialUSB.println(myIMU.magScale[2]);
    delay(2000);
  } else {
    SerialUSB.print("Could not connect to ICM20948: 0x");
    SerialUSB.println(c, HEX);
    SerialUSB.println(F("Communication failed, abort!"));
    SerialUSB.flush();
    abort();
  }
}

void loop() {
  if (myIMU.readByte(ICM20948_ADDRESS, INT_STATUS_1) & 0x01) {
    myIMU.readAccelData(myIMU.accelCount);
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes;
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes;
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes;

    myIMU.readGyroData(myIMU.gyroCount);
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;

    myIMU.readMagData(myIMU.magCount);
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes - myIMU.magBias[2];
  }

  myIMU.updateTime();
  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD, myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my, myIMU.mx, myIMU.mz, myIMU.deltat);

  if (!AHRS) {
    myIMU.delt_t = millis() - myIMU.count;
    if (myIMU.delt_t > 500) {
      if (SerialDebug) {
        SerialUSB.print("X-acceleration: ");
        SerialUSB.print(1000 * myIMU.ax);
        SerialUSB.print(" mg ");
        SerialUSB.print("Y-acceleration: ");
        SerialUSB.print(1000 * myIMU.ay);
        SerialUSB.print(" mg ");
        SerialUSB.print("Z-acceleration: ");
        SerialUSB.print(1000 * myIMU.az);
        SerialUSB.println(" mg ");

        SerialUSB.print("X-gyro rate: ");
        SerialUSB.print(myIMU.gx, 3);
        SerialUSB.print(" degrees/sec ");
        SerialUSB.print("Y-gyro rate: ");
        SerialUSB.print(myIMU.gy, 3);
        SerialUSB.print(" degrees/sec ");
        SerialUSB.print("Z-gyro rate: ");
        SerialUSB.print(myIMU.gz, 3);
        SerialUSB.println(" degrees/sec");

        SerialUSB.print("X-mag field: ");
        SerialUSB.print(myIMU.mx);
        SerialUSB.print(" mG ");
        SerialUSB.print("Y-mag field: ");
        SerialUSB.print(myIMU.my);
        SerialUSB.print(" mG ");
        SerialUSB.print("Z-mag field: ");
        SerialUSB.print(myIMU.mz);
        SerialUSB.println(" mG");

        myIMU.tempCount = myIMU.readTempData();
        myIMU.temperature = ((float)myIMU.tempCount) / 333.87 + 21.0;
        SerialUSB.print("Temperature is ");
        SerialUSB.print(myIMU.temperature, 1);
        SerialUSB.println(" degrees C");
      }

      myIMU.count = millis();
    }
  } else {
    myIMU.delt_t = millis() - myIMU.count;

    if (myIMU.delt_t > 500) {
      if (SerialDebug) {
        SerialUSB.print("ax = ");
        SerialUSB.print((int)1000 * myIMU.ax);
        SerialUSB.print(" ay = ");
        SerialUSB.print((int)1000 * myIMU.ay);
        SerialUSB.print(" az = ");
        SerialUSB.print((int)1000 * myIMU.az);
        SerialUSB.println(" mg");

        SerialUSB.print("gx = ");
        SerialUSB.print(myIMU.gx, 2);
        SerialUSB.print(" gy = ");
        SerialUSB.print(myIMU.gy, 2);
        SerialUSB.print(" gz = ");
        SerialUSB.print(myIMU.gz, 2);
        SerialUSB.println(" deg/s");

        SerialUSB.print("mx = ");
        SerialUSB.print((int)myIMU.mx);
        SerialUSB.print(" my = ");
        SerialUSB.print((int)myIMU.my);
        SerialUSB.print(" mz = ");
        SerialUSB.print((int)myIMU.mz);
        SerialUSB.println(" mG");

        SerialUSB.print("q0 = ");
        SerialUSB.print(*getQ());
        SerialUSB.print(" qx = ");
        SerialUSB.print(*(getQ() + 1));
        SerialUSB.print(" qy = ");
        SerialUSB.print(*(getQ() + 2));
        SerialUSB.print(" qz = ");
        SerialUSB.println(*(getQ() + 3));
      }

      myIMU.yaw = atan2(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() * *(getQ() + 3)), *getQ() * *getQ() + *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) - *(getQ() + 3) * *(getQ() + 3));
      myIMU.pitch = -asin(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() * *(getQ() + 2)));
      myIMU.roll = atan2(2.0f * (*getQ() * *(getQ() + 1) + *(getQ() + 2) * *(getQ() + 3)), *getQ() * *getQ() - *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) + *(getQ() + 3) * *(getQ() + 3));
      myIMU.pitch *= RAD_TO_DEG;
      myIMU.yaw *= RAD_TO_DEG;

      myIMU.yaw -= DECLINATION;
      myIMU.roll *= RAD_TO_DEG;

      if (SerialDebug) {
        SerialUSB.print("Yaw, Pitch, Roll: ");
        SerialUSB.print(myIMU.yaw, 2);
        SerialUSB.print(", ");
        SerialUSB.print(myIMU.pitch, 2);
        SerialUSB.print(", ");
        SerialUSB.println(myIMU.roll, 2);

        SerialUSB.print("rate = ");
        SerialUSB.print((float)myIMU.sumCount / myIMU.sum, 2);
        SerialUSB.println(" Hz");
      }

      myIMU.count = millis();
      myIMU.sumCount = 0;
      myIMU.sum = 0;
    }
  }
}
