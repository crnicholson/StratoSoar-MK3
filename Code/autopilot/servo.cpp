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

int prevErrorPitch, prevErrorYaw, integralPitch, integralYaw, leftTime, rightTime, rudderTime, elevatorTime, prevErrorRudder, integralRudder, prevErrorElevator, integralElevator;
unsigned long leftServoStartTime, rightServoStartTime, rudderStartTime, elevatorStartTime;
bool leftServoMoving, rightServoMoving, rudderMoving, elevatorMoving;

Servo leftServo, rightServo;
Servo rudder, elevator;

void servoSetup() {
#ifdef FLYING_WING
  leftServo.attach(LEFT_SERVO);
  rightServo.attach(RIGHT_SERVO);
#endif
#ifndef FLYING_WING
  rudder.attach(RUDDER_PIN);
  elevator.attach(ELEVATOR_PIN);
#endif
}

void pidElevons(int pitch, int turningAngle) {
  int errorPitch = SETPOINT_PITCH - pitch;

  int outputPitch = KP_LEFT * errorPitch + KI_LEFT * integralPitch + KD_LEFT * (errorPitch - prevErrorPitch);
  int outputYaw = KP_RIGHT * turningAngle + KI_RIGHT * integralYaw + KD_RIGHT * (turningAngle - prevErrorYaw);

  prevErrorPitch = errorPitch;
  integralPitch += errorPitch;

  prevErrorYaw = turningAngle;
  integralYaw += turningAngle;

  servoPositionLeft = 90 - outputPitch + outputYaw + desiredPitch;
  servoPositionRight = 90 - outputPitch - outputYaw + desiredPitch;

  // Map servo positions if needed.
  // servoPositionLeft = map(servoPositionLeft, 0, 180, 750, 2250);
  // servoPositionRight = map(servoPositionRight, 0, 180, 750, 2250);
}

int pidRudder(int turningAngle) {
  int outputRudder = KP_RUDDER * turningAngle + KI_RUDDER * integralRudder + KD_RUDDER * (turningAngle - prevErrorRudder);

  // Update previous error and integral.
  prevErrorRudder = turningAngle;
  integralRudder += turningAngle;

  // map(servoPositionRudder, 0, 180, 1250, 1750); // For servos that take microseconds, not degrees.

  return (180 + outputRudder) / 2;
}

int pidElevator(int pitch) {
  int errorElevator = SETPOINT_ELEVATOR - pitch; // Calculate the error.

  int outputElevator = KP_ELEVATOR * errorElevator + KI_ELEVATOR * integralElevator + KD_ELEVATOR * (errorElevator - prevErrorElevator);

  // Update previous error and integral.
  prevErrorElevator = errorElevator;
  integralElevator += errorElevator;

  // map(servoPositionElevator2, 0, 180, 750, 2250); // For servos that take microseconds, not degrees.

  return 90 - outputElevator;
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

void startLeftServo(int degrees) {
  digitalWrite(LEFT_FET, HIGH); // Turn servo on.

  int leftChange = abs(leftServo.read() - degrees);
  leftTime = leftChange * 170 / 60;
  if (leftTime < 40) {
    leftTime += 40;
  }
  if (leftTime < 60) {
    leftTime += 20;
  }

  leftServo.write(degrees);
  leftServoStartTime = millis();
  leftServoMoving = true;
}

void updateLeftServo() {
  if ((millis() - leftServoStartTime > leftTime) && leftServoMoving) {
    digitalWrite(LEFT_FET, LOW);
    leftServoMoving = false;
  }
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

void startRightServo(int degrees) {
  digitalWrite(RIGHT_FET, HIGH); // Turn servo on.

  int rightChange = abs(rightServo.read() - degrees);
  rightTime = rightChange * 170 / 60;
  if (rightTime < 40) {
    rightTime += 40;
  }
  if (rightTime < 60) {
    rightTime += 20;
  }

  rightServo.write(degrees);
  rightServoStartTime = millis();
  rightServoMoving = true;
}

void updateRightServo() {
  if ((millis() - rightServoStartTime > rightTime) && rightServoMoving) {
    digitalWrite(RIGHT_FET, LOW);
    rightServoMoving = false;
  }
}

void moveRudder(int degrees) {
  digitalWrite(RUDDER_FET, HIGH); // Turn servo on.
  int rudderLast = rudder.read();
  int rudderChange = abs(rudderLast - degrees);
  int rudderTime = rudderChange * 170 / 60;
  if (rudderTime < 40) {
    rudderTime = rudderTime + 40;
  }
  if (rudderTime < 60) {
    rudderTime = rudderTime + 20;
  }
  rudder.write(degrees);
  delay(rudderTime);
  digitalWrite(RUDDER_FET, LOW); // Turn servo off.
}

void startRudder(int degrees) {
  digitalWrite(RUDDER_FET, HIGH); // Turn servo on.

  int rudderChange = abs(rudder.read() - degrees);
  rudderTime = rudderChange * 170 / 60;
  if (rudderTime < 40) {
    rudderTime += 40;
  }
  if (rudderTime < 60) {
    rudderTime += 20;
  }

  rudder.write(degrees);
  rudderStartTime = millis();
  rudderMoving = true;
}

void updateRudder() {
  if ((millis() - rudderStartTime > rudderTime) && rudderMoving) {
    digitalWrite(RIGHT_FET, LOW);
    rudderMoving = false;
  }
}

void moveElevator(int degrees) {
  digitalWrite(ELEVATOR_FET, HIGH); // Turn servo on.
  int elevatorLast = elevator.read();
  int elevatorChange = abs(elevatorLast - degrees);
  int elevatorTime = elevatorChange * 170 / 60;
  if (elevatorTime < 40) {
    elevatorTime = elevatorTime + 40;
  }
  if (elevatorTime < 60) {
    elevatorTime = elevatorTime + 20;
  }
  elevator.write(degrees);
  delay(elevatorTime);
  digitalWrite(ELEVATOR_FET, LOW); // Turn servo off.
}

void startElevator(int degrees) {
  digitalWrite(ELEVATOR_FET, HIGH); // Turn servo on.

  int elevatorChange = abs(elevator.read() - degrees);
  elevatorTime = elevatorChange * 170 / 60;
  if (elevatorTime < 40) {
    elevatorTime += 40;
  }
  if (elevatorTime < 60) {
    elevatorTime += 20;
  }

  elevator.write(degrees);
  elevatorStartTime = millis();
  elevatorMoving = true;
}

void updateElevator() {
  if ((millis() - elevatorStartTime > elevatorTime) && elevatorMoving) {
    digitalWrite(ELEVATOR_FET, LOW);
    elevatorMoving = false;
  }
}

// void land(int left, int right) {
//   moveLeftServo(left);
//   moveRightServo(right);
// }

// Experimental stuff below.

// This program will return the best pitch to fly at for most distance, found experimentally.
float findBestPitch() {
  float results[] = {};
  float bestPitch = 0.0;
  float lastRate = 0.0;
  int x = 0;
  int y = 0;
  while (x < PITCH_RANGE) {
    getGPSData();
    float prevLat = lat;
    float prevLon = lon;
    float prevAlt = altitude;
    moveLeftServo(x + STARTING_PITCH);
    moveRightServo(x + STARTING_PITCH);
    long beforeTest = millis();
    while (beforeTest + STEP_TIME < millis()) {
      longBlink(LED); // Just blink the LED.
    }
    getGPSData();
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

#define potPin A0
#define motorPin 9
#define motorFeedbackPin A1

#define KP 2.0
#define KI 0.5
#define KD 1.0

double previousError, integral;
unsigned long lastTime;

void experiementalServo(int setpoint) {
  double input, output;
  int potValue = analogRead(potPin);

  setpoint = map(potValue, 0, 1023, 0, 180);

  int motorFeedbackValue = analogRead(motorFeedbackPin);
  input = map(motorFeedbackValue, 0, 1023, 0, 180);

  unsigned long currentTime = millis();
  double timeChange = (double)(currentTime - lastTime);

  double error = setpoint - input;
  integral += (error * timeChange);
  double derivative = (error - previousError) / timeChange;

  output = (KP * error) + (KI * integral) + (KD * derivative);

  output = constrain(output, 0, 255);

  analogWrite(motorPin, output);

  previousError = error;
  lastTime = currentTime;

#ifdef DEVMODE
  SerialUSB.print("Setpoint: ");
  SerialUSB.print(setpoint);
  SerialUSB.print(" | Input: ");
  SerialUSB.print(input);
  SerialUSB.print(" | Output: ");
  SerialUSB.println(output);
#endif

  delay(50);
}
