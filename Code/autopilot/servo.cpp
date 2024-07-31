/*
servo.cpp, part of StratoSoar MK3, for an autonomous glider.
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

#include "servo.h"

#define SETPOINT_PITCH 0

int errorPitch, errorYaw, prevErrorPitch, prevErrorYaw, integralPitch, integralYaw;

Servo leftServo, rightServo;

void servoSetup() {
  leftServo.attach(LEFT_SERVO);
  rightServo.attach(RIGHT_SERVO);
}

int pidElevons(int pitch, int yaw, int turningAngle) {
  errorPitch = SETPOINT_PITCH - pitch;
  errorYaw = turningAngle;

  int outputPitch = KP_LEFT * errorPitch + KI_LEFT * integralPitch + KD_LEFT * (errorPitch - prevErrorPitch);
  int outputYaw = KP_RIGHT * errorYaw + KI_RIGHT * integralYaw + KD_RIGHT * (errorYaw - prevErrorYaw);

  prevErrorPitch = errorPitch;
  integralPitch += errorPitch;

  prevErrorYaw = errorYaw;
  integralYaw += errorYaw;

  int servoPositionLeft = 90 - outputPitch + outputYaw + desiredPitch;
  int servoPositionRight = 90 - outputPitch - outputYaw + desiredPitch;

  // Map servo positions if needed.
  // servoPositionLeft = map(servoPositionLeft, 0, 180, 750, 2250);
  // servoPositionRight = map(servoPositionRight, 0, 180, 750, 2250);

  return servoPositionLeft, servoPositionRight;
}

void moveLeftServo(int degrees) {
  digitalWrite(LEFT_FET, HIGH); // Turn servo on.
  int leftLast = leftServo.read();
  int leftChange = abs(leftLast - degrees);
  int leftTime = leftChange * 170 / 60;
  if (leftTime < 40) {
    leftTime = leftTime + 40;
  }
  if (leftTime < 60) {
    leftTime = leftTime + 20;
  }
  leftServo.write(degrees);
  delay(leftTime);
  digitalWrite(LEFT_FET, LOW); // Turn servo off.
}

void moveRightServo(int degrees) {
  digitalWrite(RIGHT_FET, HIGH); // Turn servo on.
  int rightLast = rightServo.read();
  int rightChange = abs(rightLast - degrees);
  int rightTime = rightChange * 170 / 60;
  if (rightTime < 40) {
    rightTime = rightTime + 40;
  }
  if (rightTime < 60) {
    rightTime = rightTime + 20;
  }
  rightServo.write(degrees);
  delay(rightTime);
  digitalWrite(RIGHT_FET, LOW); // Turn servo off.
}

void land(int left, int right) {
  moveLeftServo(left);
  moveRightServo(right);
}

// This program will return the best pitch to fly at for most distance, found experimentally.
float findBestPitch() {
  float results[] = {};
  float bestPitch = 0.0;
  float lastRate = 0.0;
  int x = 0;
  int y = 0;
  while (x < PITCH_RANGE) {
    lat, lon, altitude, year, month, day, hour, minute, second = getGPSData();
    float prevLat = lat;
    float prevLon = lon;
    float prevAlt = altitude;
    moveLeftServo(x + STARTING_PITCH);
    moveRightServo(x + STARTING_PITCH);
    long beforeTest = millis();
    while (beforeTest + STEP_TIME < millis()) {
      longBlink(LED); // Just blink the LED.
    }
    lat, lon, altitude, year, month, day, hour, minute, second = getGPSData();
    float distance = calculateDistance(prevLat, prevLon, lat, lon);
    float glideRate = distance / altitude;

    results[y] = x + STARTING_PITCH;
    y++;
    results[y] = glideRate;
    y++;

    if (glideRate > lastRate) {
      bestPitch = STARTING_PITCH + x;
    }

    x++;
  }
#ifdef DEVMODE
  // SerialUSB.println(results);
#endif
  return bestPitch;
}

#define GRAMS 100
#define SEA_LEVEL_AIR_DENSITY 1.225 // kg/m^3
#define C_L 1                       // We can estimate this to be 1.
#define SA 100                      // Plan form surface area of the wing in square decimeters.

float calculateStallSpeed(float altitude) {
  float w = GRAMS * 0.009806652;
  float rho = SEA_LEVEL_AIR_DENSITY * exp(-altitude / 8000.0); // Approximation
  return sqrt((2 * w) / (rho * SA * 100 * C_L));
}