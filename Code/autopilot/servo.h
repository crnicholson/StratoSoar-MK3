#pragma once

#include "settings.h"
#include <Arduino.h>
#include <Servo.h>

int pidElevons(int pitch, int yaw);
void moveLeftServo(int servoPositionLeft);
void moveRightServo(int servoPositionRight);