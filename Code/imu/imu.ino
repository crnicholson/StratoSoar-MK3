/* ICM20948 Basic Example Code
 by: Kris Winer
 date: April 1, 2014
 license: Beerware - Use this code however you'd like. If you
 find it useful you can buy me a beer some time.
 Modified by Brent Wilkins July 19, 2016

 Demonstrate basic ICM20948 functionality including parameterizing the register
 addresses, initializing the sensor, getting properly scaled accelerometer,
 gyroscope, and magnetometer data out. Added display functions to allow display
 to on breadboard monitor. Addition of 9 DoF sensor fusion using open source
 Madgwick and Mahony filter algorithms. Sketch runs on the 3.3 V 8 MHz Pro Mini
 and the Teensy 3.1.
*/

#include "AHRSAlgorithms.h"
#include "ICM20948.h"
#include "settings.h"

#define AHRS true        // Set to false for basic data read
#define SerialDebug true // Set to true to get Serial output for debugging

// Pin definitions
int myLed = 13; // Set up pin 13 led for toggling

ICM20948 myIMU;

void setup() {
  Wire.begin();
  // TWBR = 12;  // 400 kbit/sec I2C speed
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, HIGH);

  // Reset ICM20948.
  myIMU.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, READ_FLAG);
  delay(100);
  myIMU.writeByte(ICM20948_ADDRESS, PWR_MGMT_1, 0x01);
  delay(100);

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(ICM20948_ADDRESS, WHO_AM_I_ICM20948);
  Serial.print(F("ICM20948 I AM 0x"));
  Serial.print(c, HEX);
  Serial.print(F(" I should be 0x"));
  Serial.println(0xEA, HEX);

  if (c == 0xEA) {
    // WHO_AM_I should always be 0x71
    Serial.println(F("ICM20948 is online..."));

    // Start by performing self test and reporting values
    myIMU.ICM20948SelfTest(myIMU.selfTest);
    Serial.print(F("x-axis self test: acceleration trim within : "));
    Serial.print(myIMU.selfTest[0], 1);
    Serial.println("% of factory value");
    Serial.print(F("y-axis self test: acceleration trim within : "));
    Serial.print(myIMU.selfTest[1], 1);
    Serial.println("% of factory value");
    Serial.print(F("z-axis self test: acceleration trim within : "));
    Serial.print(myIMU.selfTest[2], 1);
    Serial.println("% of factory value");
    Serial.print(F("x-axis self test: gyration trim within : "));
    Serial.print(myIMU.selfTest[3], 1);
    Serial.println("% of factory value");
    Serial.print(F("y-axis self test: gyration trim within : "));
    Serial.print(myIMU.selfTest[4], 1);
    Serial.println("% of factory value");
    Serial.print(F("z-axis self test: gyration trim within : "));
    Serial.print(myIMU.selfTest[5], 1);
    Serial.println("% of factory value");

    // Calibrate gyro and accelerometers, load biases in bias registers
    myIMU.calibrateICM20948(myIMU.gyroBias, myIMU.accelBias);

    myIMU.initICM20948();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    Serial.println("ICM20948 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK09916_ADDRESS, WHO_AM_I_AK09916);
    Serial.print("AK8963 ");
    Serial.print("I AM 0x");
    Serial.print(d, HEX);
    Serial.print(" I should be 0x");
    Serial.println(0x09, HEX);

    if (d != 0x09) {
      // Communication failed, stop here
      Serial.println(F("Communication with magnetometer failed, abort!"));
      Serial.flush();
      abort();
    }

    // Get magnetometer calibration from AK8963 ROM
    myIMU.initAK09916();
    // Initialize device for active mode read of magnetometer
    Serial.println("AK09916 initialized for active data mode....");

    /*
      if (SerialDebug)
      {
        //  Serial.println("Calibration values: ");
        Serial.print("X-Axis factory sensitivity adjustment value ");
        Serial.println(myIMU.factoryMagCalibration[0], 2);
        Serial.print("Y-Axis factory sensitivity adjustment value ");
        Serial.println(myIMU.factoryMagCalibration[1], 2);
        Serial.print("Z-Axis factory sensitivity adjustment value ");
        Serial.println(myIMU.factoryMagCalibration[2], 2);
      }
    */

    // Get sensor resolutions, only need to do this once
    myIMU.getAres();
    myIMU.getGres();
    myIMU.getMres();

    // The next call delays for 4 seconds, and then records about 15 seconds of
    // data to calculate bias and scale.
    myIMU.magCalICM20948(myIMU.magBias, myIMU.magScale);
    Serial.println("AK09916 mag biases (mG)");
    Serial.println(myIMU.magBias[0]);
    Serial.println(myIMU.magBias[1]);
    Serial.println(myIMU.magBias[2]);

    Serial.println("AK09916 mag scale (mG)");
    Serial.println(myIMU.magScale[0]);
    Serial.println(myIMU.magScale[1]);
    Serial.println(myIMU.magScale[2]);
    delay(2000); // Add delay to see results before serial spew of data
  } // if (c == 0x71)
  else {
    Serial.print("Could not connect to ICM20948: 0x");
    Serial.println(c, HEX);

    // Communication failed, stop here
    Serial.println(F("Communication failed, abort!"));
    Serial.flush();
    abort();
  }
}

void loop() {
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (myIMU.readByte(ICM20948_ADDRESS, INT_STATUS_1) & 0x01) {
    myIMU.readAccelData(myIMU.accelCount); // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes; // - myIMU.accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes; // - myIMU.accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes; // - myIMU.accelBias[2];

    myIMU.readGyroData(myIMU.gyroCount); // Read the x/y/z adc values

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;

    myIMU.readMagData(myIMU.magCount); // Read the x/y/z adc values

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes - myIMU.magBias[2];
  } // if (readByte(ICM20948_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  myIMU.updateTime();

  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // ICM20948, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);

  if (!AHRS) {
    myIMU.delt_t = millis() - myIMU.count;
    if (myIMU.delt_t > 500) {
      if (SerialDebug) {
        // Print acceleration values in milligs!
        Serial.print("X-acceleration: ");
        Serial.print(1000 * myIMU.ax);
        Serial.print(" mg ");
        Serial.print("Y-acceleration: ");
        Serial.print(1000 * myIMU.ay);
        Serial.print(" mg ");
        Serial.print("Z-acceleration: ");
        Serial.print(1000 * myIMU.az);
        Serial.println(" mg ");

        // Print gyro values in degree/sec
        Serial.print("X-gyro rate: ");
        Serial.print(myIMU.gx, 3);
        Serial.print(" degrees/sec ");
        Serial.print("Y-gyro rate: ");
        Serial.print(myIMU.gy, 3);
        Serial.print(" degrees/sec ");
        Serial.print("Z-gyro rate: ");
        Serial.print(myIMU.gz, 3);
        Serial.println(" degrees/sec");

        // Print mag values in degree/sec
        Serial.print("X-mag field: ");
        Serial.print(myIMU.mx);
        Serial.print(" mG ");
        Serial.print("Y-mag field: ");
        Serial.print(myIMU.my);
        Serial.print(" mG ");
        Serial.print("Z-mag field: ");
        Serial.print(myIMU.mz);
        Serial.println(" mG");

        myIMU.tempCount = myIMU.readTempData(); // Read the adc values
        // Temperature in degrees Centigrade
        myIMU.temperature = ((float)myIMU.tempCount) / 333.87 + 21.0;
        // Print temperature in degrees Centigrade
        Serial.print("Temperature is ");
        Serial.print(myIMU.temperature, 1);
        Serial.println(" degrees C");
      }

      myIMU.count = millis();
      digitalWrite(myLed, !digitalRead(myLed)); // toggle led
    } // if (myIMU.delt_t > 500)
  } // if (!AHRS)
  else {
    // Serial print and/or display at 0.5 s rate independent of data rates
    myIMU.delt_t = millis() - myIMU.count;

    // update LCD once per half-second independent of read rate
    if (myIMU.delt_t > 500) {
      if (SerialDebug) {
        Serial.print("ax = ");
        Serial.print((int)1000 * myIMU.ax);
        Serial.print(" ay = ");
        Serial.print((int)1000 * myIMU.ay);
        Serial.print(" az = ");
        Serial.print((int)1000 * myIMU.az);
        Serial.println(" mg");

        Serial.print("gx = ");
        Serial.print(myIMU.gx, 2);
        Serial.print(" gy = ");
        Serial.print(myIMU.gy, 2);
        Serial.print(" gz = ");
        Serial.print(myIMU.gz, 2);
        Serial.println(" deg/s");

        Serial.print("mx = ");
        Serial.print((int)myIMU.mx);
        Serial.print(" my = ");
        Serial.print((int)myIMU.my);
        Serial.print(" mz = ");
        Serial.print((int)myIMU.mz);
        Serial.println(" mG");

        Serial.print("q0 = ");
        Serial.print(*getQ());
        Serial.print(" qx = ");
        Serial.print(*(getQ() + 1));
        Serial.print(" qy = ");
        Serial.print(*(getQ() + 2));
        Serial.print(" qz = ");
        Serial.println(*(getQ() + 3));
      }

      myIMU.yaw = atan2(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() * *(getQ() + 3)), *getQ() * *getQ() + *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) - *(getQ() + 3) * *(getQ() + 3));
      myIMU.pitch = -asin(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() * *(getQ() + 2)));
      myIMU.roll = atan2(2.0f * (*getQ() * *(getQ() + 1) + *(getQ() + 2) * *(getQ() + 3)), *getQ() * *getQ() - *(getQ() + 1) * *(getQ() + 1) - *(getQ() + 2) * *(getQ() + 2) + *(getQ() + 3) * *(getQ() + 3));
      myIMU.pitch *= RAD_TO_DEG;
      myIMU.yaw *= RAD_TO_DEG;

      myIMU.yaw -= DECLINATION;
      myIMU.roll *= RAD_TO_DEG;

      if (SerialDebug) {
        Serial.print("Yaw, Pitch, Roll: ");
        Serial.print(myIMU.yaw, 2);
        Serial.print(", ");
        Serial.print(myIMU.pitch, 2);
        Serial.print(", ");
        Serial.println(myIMU.roll, 2);

        Serial.print("rate = ");
        Serial.print((float)myIMU.sumCount / myIMU.sum, 2);
        Serial.println(" Hz");
      }

      myIMU.count = millis();
      myIMU.sumCount = 0;
      myIMU.sum = 0;
    } // if (myIMU.delt_t > 500)
  } // if (AHRS)
}