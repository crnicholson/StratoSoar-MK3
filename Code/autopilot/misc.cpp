/*
misc.cpp, part of StratoSoar MK3, for an autonomous glider.
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

#include "misc.h"

float readVoltage() {
  int rawVolt = analogRead(VOLTMETER_PIN);
  rawVolt = rawVolt * 2;
  return rawVolt * (3.3 / 1023.0);
}

void printData() {
  SerialUSB.print("Lat: ");
  SerialUSB.print(lat, 7);
  SerialUSB.print(" Lon: ");
  SerialUSB.print(lon, 7);
  SerialUSB.print(" Altitude (Meters): ");
  SerialUSB.print(altitude);
  SerialUSB.print(" Distance to Target: ");
  SerialUSB.print(distance);
  SerialUSB.print(" Yaw: ");
  SerialUSB.print(yaw);
  SerialUSB.print(" Pitch: ");
  SerialUSB.print(pitch);
  SerialUSB.print(" Roll: ");
  SerialUSB.print(roll);
  SerialUSB.print(" Turning Angle: ");
  SerialUSB.print(turnAngle);
  SerialUSB.print(" Left Servo Position: ");
  SerialUSB.print(servoPositionLeft);
  SerialUSB.print(" Right Servo Position: ");
  SerialUSB.print(servoPositionRight);
  SerialUSB.print(" Date/Time: ");
  SerialUSB.print(year);
  SerialUSB.print("-");
  SerialUSB.print(month);
  SerialUSB.print("-");
  SerialUSB.print(day);
  SerialUSB.print(" ");
  SerialUSB.print(hour);
  SerialUSB.print(":");
  SerialUSB.print(minute);
  SerialUSB.print(":");
  SerialUSB.print(second);
  SerialUSB.print(" Temperature: ");
  SerialUSB.print(temperature);
  SerialUSB.print(" Humidity: ");
  SerialUSB.print(humidity);
  SerialUSB.print(" Pressure: ");
  SerialUSB.print(pressure);
  SerialUSB.print(" Altitude (BME): ");
  SerialUSB.println(bmeAltitude);
}
