/*
servo.h, part of StratoSoar MK3, for an autonomous glider.
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

#pragma once

#include "calc.h"
#include "gps.h"
#include "settings.h"
#include "vars.h"
#include <Arduino.h>
#include <Servo.h>

void servoSetup();
int pidElevons(int pitch, int yaw, int turningAngle);
void moveLeftServo(int degrees);
void moveRightServo(int degrees);
void land(int left, int right);
float findBestPitch();