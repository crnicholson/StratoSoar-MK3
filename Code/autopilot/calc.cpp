/*
calc.cpp, part of StratoSoar MK3, for an autonomous glider.
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

#include "calc.h"

double toRadians(double deg) {
  return deg * PI / 180.0;
}

double toDegrees(double rad) {
  return rad * 180.0 / PI;
}

double azimuth(double cLat, double cLon, double tLat, double tLon) {
  double dLon = toRadians(tLon - cLon);
  double dPhi = log(tan(toRadians(tLat) / 2 + PI / 4) / tan(toRadians(cLat) / 2 + PI / 4));
  if (fabs(dLon) > PI) {
    dLon = dLon > 0 ? -(2 * PI - dLon) : (2 * PI + dLon);
  }
  return fmod((atan2(dLon, dPhi) + 2 * PI), (2 * PI));
}

double turningAngle(double cLat, double cLon, double heading, double tLat, double tLon) {
  double a = azimuth(cLat, cLon, tLat, tLon);
  double h = toRadians(heading);
  double angle = a - h;
  if (angle < -PI)
    angle += 2 * PI;
  if (angle > PI)
    angle -= 2 * PI;
  return toDegrees(angle);
}

// Haversine formula to calculate distance between two coordinates.
double calculateDistance(double cLat, double cLon, double tLat, double tLon) {
  double dLat = toRadians(tLat - cLat);
  double dLon = toRadians(tLon - cLon);
  double a = sin(dLat / 2) * sin(dLat / 2) + cos(toRadians(tLat)) * cos(toRadians(tLat)) * sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double distance = 6371000 * c;
  return distance;
}