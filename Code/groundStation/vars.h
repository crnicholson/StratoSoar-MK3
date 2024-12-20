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

bool dataValid, newPacketForGlider;
long rxCount;

struct fromGlider {
  byte alignment;
  float lat, lon, tLat, tLon, altitude, temperature, pressure, humidity, volts, hdop;
  short yaw, pitch, roll;
  byte year, month, day, hour, minute, second;
  short txCount;
  bool abort;
  int password;
  float user1, user2, user3, user4, user5;
  char callSign[7];
};

extern struct fromGlider fromGliderStruct;

struct toGlider {
  float tLat, tLon;
  bool abort;
  char callSign[7];
};

extern struct toGlider toGliderStruct;
