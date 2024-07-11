#pragma once

// GPS vars.
extern float lat, lon, altitude;
extern int year, month, day, hour, minute, second;
extern long gpsLast;

// Navigation/IMU vars.
extern int yaw, pitch, roll, turnAngle, servoPositionLeft, servoPositionRight;
extern long distance;