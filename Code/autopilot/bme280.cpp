/*
bme280.cpp, part of StratoSoar MK3, for an autonomous glider.
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

#include "bme280.h"

// Altitude in meters.
float bme280Altitude(float referencePressure) {
  return ((float)-45846.2) * (pow(((float)(BME280pressure()) / (float)referencePressure), 0.190263) - (float)1); // referencePressure is the pressure in hPa at zero altitude; for example, 1013.250.
}

void getBMEData(float referencePressure) {
  // Temperature (C), humidity (RH%), pressure (hPa), and altitude (m).
  temperature = BME280temperature();
  humidity = BME280humidity();
  pressure = BME280pressure();
  bmeAltitude = bme280Altitude(referencePressure);
}