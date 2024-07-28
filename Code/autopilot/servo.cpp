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

#define KP_LEFT 1
#define KI_LEFT 0
#define KD_LEFT 0

#define KP_RIGHT 1
#define KI_RIGHT 0
#define KD_RIGHT 0

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

  int servoPositionLeft = 90 - outputPitch + outputYaw;
  int servoPositionRight = 90 - outputPitch - outputYaw;

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