/*
gps.h, part of StratoSoar MK3, for an autonomous glider.
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

#include "blink.h"
#include "settings.h"
#include "vars.h"
#include <Arduino.h>
#include <TinyGPSPlus.h>

extern TinyGPSPlus gps;

void gpsSetup();
void getGPSData();
void getGPSLocation();
void getGPSTime();
void getGPSDate();
void gpsSleepTime(long ms);
void gpsSleep();
void gpsWakeup(bool wait = true);
void waitForFix();