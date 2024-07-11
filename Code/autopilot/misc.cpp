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
  SerialUSB.println(second);
}
