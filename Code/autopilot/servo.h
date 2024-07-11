#pragma once

#include "settings.h"
#include <Arduino.h>
#include <Servo.h>

void servoSetup();
int pidElevons(int pitch, int yaw, int turningAngle);
void moveLeftServo(int degrees);
void moveRightServo(int degrees);