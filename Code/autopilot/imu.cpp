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

ICM_20948_I2C imu;

// Don't change. Tuning parameters. IMU_KP is not yet optimized (slight overshoot apparent after rapid sensor reorientations). IMU_KI is not used.
#define IMU_KP 50.0
#define IMU_KI 0.0

#define PRINT_SPEED 1300

// Variables.
unsigned long imuNow, lastIMUUpdate; // For counting time between updates.
float deltat = 0;                    // For counting time between updates in microseconds.

int hertz; // For measuring frequency of sensor.

float q[4] = {1.0, 0.0, 0.0, 0.0};           // Vector to hold quaternion.
float Gxyz[3], Axyz[3], Mxyz[3];             // Centered and scaled gyro/accel/mag data.
float integralError[3] = {0.0f, 0.0f, 0.0f}; // Integral error for Mahony method.
bool imuInit, magInit;                       // Bools for mag and IMU intialization.

float Gscale = (M_PI / 180.0) * 0.00763; // 250 dps scale sensitivity = 131 dps/LSB

void imuSetup() {
  while (!imuInit || !magInit) {
    imu.begin(Wire, AD0_VAL);

#ifdef DEVMODE
    SerialUSB.print("Initialization of the IMU returned: ");
    SerialUSB.println(imu.statusString());
#endif
    if (imu.status != ICM_20948_Stat_Ok) {
#ifdef DEVMODE
      SerialUSB.println("Trying again...");
#endif
      delay(500);
    } else {
      imuInit = true;
    }

    imu.startupMagnetometer();
#ifdef DEVMODE
    SerialUSB.print(F("Initialization of the magnetometer returned: "));
    SerialUSB.println(imu.statusString());
#endif
    if (imu.status != ICM_20948_Stat_Ok) {
#ifdef DEVMODE
      SerialUSB.println("Trying again...");
#endif
      delay(500);
    } else {
      magInit = true;
    }
  }
}

void getAHRS() {
  // Converting the quarternions to Euler angles (radians). This angular conversion can fail. See gimbal lock.
  roll = atan2((q[0] * q[1] + q[2] * q[3]), 0.5 - (q[1] * q[1] + q[2] * q[2]));
  pitch = asin(2.0 * (q[0] * q[2] - q[1] * q[3]));
  yaw = atan2((q[1] * q[2] + q[0] * q[3]), 0.5 - (q[2] * q[2] + q[3] * q[3]));

  // Converting radians to degrees.
  yaw *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll *= 180.0 / PI;

  // // Accounting for declination.
  yaw = -(yaw + DECLINATION);
  if (yaw < 0)
    yaw += 360.0;
  if (yaw >= 360.0)
    yaw -= 360.0;

  int freq = hertz / (PRINT_SPEED / 1000);

  // SerialUSB.println();
  // SerialUSB.println(freq); // Calculating the frequency of the sensor.
  // SerialUSB.println();

  hertz = 0; // Reset counter for next print.
}

void imuMath() {
  if (imu.dataReady()) {
    imu.getAGMT(); // Get accelerometer, gyro, mag, and temperature data and put it into the imu.agmt data structure.

    hertz++;

    getScaledIMU(Gxyz, Axyz, Mxyz); // Scale the raw values of the IMU.

    Mxyz[1] = -Mxyz[1]; // Reflect Y and Z.
    Mxyz[2] = -Mxyz[2]; // Must be done after offsets & scales applied to raw data.

    imuNow = micros();
    deltat = (imuNow - lastIMUUpdate) * 1.0e-6; // Get microseconds since last update.
    lastIMUUpdate = imuNow;

    quaternionUpdate(Axyz[0], Axyz[1], Axyz[2], Gxyz[0], Gxyz[1], Gxyz[2], Mxyz[0], Mxyz[1], Mxyz[2], deltat); // Updates the quaternion array with the scaled values.
  }
}

float vectorDot(float a[3], float b[3]) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void vectorNormalize(float a[3]) {
  float mag = sqrt(vectorDot(a, a));
  a[0] /= mag;
  a[1] /= mag;
  a[2] /= mag;
}

// Function to apply scale and offset corrections to IMU data.
void getScaledIMU(float Gxyz[3], float Axyz[3], float Mxyz[3]) {
  float temp[3];

  // Apply the gyroscope scale and remove offsets. Gyro needs to be scaled and such because of its drift.
  Gxyz[0] = Gscale * (imu.agmt.gyr.axes.x - G_offset[0]);
  Gxyz[1] = Gscale * (imu.agmt.gyr.axes.y - G_offset[1]);
  Gxyz[2] = Gscale * (imu.agmt.gyr.axes.z - G_offset[2]);

  // Retrieve the raw accelerometer and magnetometer data.
  Axyz[0] = imu.agmt.acc.axes.x;
  Axyz[1] = imu.agmt.acc.axes.y;
  Axyz[2] = imu.agmt.acc.axes.z;
  Mxyz[0] = imu.agmt.mag.axes.x;
  Mxyz[1] = imu.agmt.mag.axes.y;
  Mxyz[2] = imu.agmt.mag.axes.z;

  // Apply the accelerometer corrections.
  for (int i = 0; i < 3; i++) {
    temp[i] = Axyz[i] - A_B[i]; // Subtract accelerometer bias.
  }

  // Correct scaling and alignment using correction matrix A_Ainv.
  Axyz[0] = A_Ainv[0][0] * temp[0] + A_Ainv[0][1] * temp[1] + A_Ainv[0][2] * temp[2];
  Axyz[1] = A_Ainv[1][0] * temp[0] + A_Ainv[1][1] * temp[1] + A_Ainv[1][2] * temp[2];
  Axyz[2] = A_Ainv[2][0] * temp[0] + A_Ainv[2][1] * temp[1] + A_Ainv[2][2] * temp[2];

  // Normalize accelerometer vector.
  vectorNormalize(Axyz);

  // Apply magnetometer corrections.
  for (int i = 0; i < 3; i++) {
    temp[i] = Mxyz[i] - M_B[i]; // Subtract magnetometer bias.
  }

  // Correct scaling and alignment using correction matrix M_Ainv.
  Mxyz[0] = M_Ainv[0][0] * temp[0] + M_Ainv[0][1] * temp[1] + M_Ainv[0][2] * temp[2];
  Mxyz[1] = M_Ainv[1][0] * temp[0] + M_Ainv[1][1] * temp[1] + M_Ainv[1][2] * temp[2];
  Mxyz[2] = M_Ainv[2][0] * temp[0] + M_Ainv[2][1] * temp[1] + M_Ainv[2][2] * temp[2];

  // Normalize magnetometer vector.
  vectorNormalize(Mxyz);
}

// Mahony orientation filter, assumed World Frame NWU (xNorth, yWest, zUp). Modified from Madgwick version to remove Z component of magnetometer.
void quaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz, float deltaTime) {
  // Extract quaternion components for readability.
  float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];

  // Auxiliary variables to avoid repeated calculations.
  float q1Squared = q1 * q1;
  float q2Squared = q2 * q2;
  float q3Squared = q3 * q3;
  float q4Squared = q4 * q4;

  // Measured horizon vector (in body frame).
  float hx = ay * mz - az * my;
  float hy = az * mx - ax * mz;
  float hz = ax * my - ay * mx;

  // Normalize horizon vector.
  float norm = sqrt(hx * hx + hy * hy + hz * hz);
  if (norm == 0.0f)
    return; // Avoid division by zero.

  hx /= norm; // Normalize.
  hy /= norm;
  hz /= norm;

  // Estimated Up vector.
  float ux = 2.0f * (q2 * q4 - q1 * q3);
  float uy = 2.0f * (q1 * q2 + q3 * q4);
  float uz = q1Squared - q2Squared - q3Squared + q4Squared;

  // Estimated West vector.
  float wx = 2.0f * (q2 * q3 + q1 * q4);
  float wy = q1Squared - q2Squared + q3Squared - q4Squared;
  float wz = 2.0f * (q3 * q4 - q1 * q2);

  // Calculate orientation errors.
  float ex = (ay * uz - az * uy) + (hy * wz - hz * wy);
  float ey = (az * ux - ax * uz) + (hz * wx - hx * wz);
  float ez = (ax * uy - ay * ux) + (hx * wy - hy * wx);

  // Integral feedback.
  if (IMU_KI > 0.0f) {
    integralError[0] += ex; // Accumulate integral error.
    integralError[1] += ey;
    integralError[2] += ez;
    gx += IMU_KI * integralError[0];
    gy += IMU_KI * integralError[1];
    gz += IMU_KI * integralError[2];
  }

  // Proportional feedback.
  gx += IMU_KP * ex;
  gy += IMU_KP * ey;
  gz += IMU_KP * ez;

  // Update quaternion with integrated gyroscope values.
  gx *= (0.5f * deltaTime); // Pre-multiply common factors.
  gy *= (0.5f * deltaTime);
  gz *= (0.5f * deltaTime);

  // Quaternion update.
  float qa = q1;
  float qb = q2;
  float qc = q3;

  q1 += (-qb * gx - qc * gy - q4 * gz);
  q2 += (qa * gx + qc * gz - q4 * gy);
  q3 += (qa * gy - qb * gz + q4 * gx);
  q4 += (qa * gz + qb * gy - qc * gx);

  // Normalize quaternion.
  norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
  norm = 1.0f / norm;
  q[0] = q1 * norm;
  q[1] = q2 * norm;
  q[2] = q3 * norm;
  q[3] = q4 * norm;
}