/*
vars.h, part of StratoSoar MK3, for an autonomous glider.
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

#include <Arduino.h>

// GPS vars.
extern float lat, lon, altitude, targetLat, targetLon, prevTLat, prevTLon, hdop;
extern int year, month, day, hour, minute, second, gpsLast;

// Navigation/IMU vars.
extern int turnAngle, servoPositionLeft, servoPositionRight, distance;
extern float yaw, pitch, roll;

// Environmental vars.
extern float temperature, pressure, bmeAltitude;
extern int humidity;

struct data {
  float lat, lon, tLat, tLon, altitude, temperature, pressure, humidity, volts, hdop;
  short yaw, pitch, roll;
  byte hour, minute, second;
  short txCount;
  byte abortFlight;
  char callSign[7] = CALL_SIGN;
};

extern struct data packet;

// Other vars.
extern int lastLoRa, abortCounter, loraUpdateRate, updateRate, lastUpdate;
extern bool abortFlight, loraAbortFlight, altitudeLock, lowVoltage, ultraLowVoltage;
extern float desiredPitch, voltage;