#include "blink.h"
#include "calc.h"
#include "gps.h"
#include "i2c_scan.h"
#include "imu.h"
#include "misc.h"
#include "servo.h"
#include "settings.h"
#include "vars.h"

void setup() {
  Wire.begin();

  pinMode(GPS_SLEEP_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ERR_LED, OUTPUT);
  pinMode(VOLTMETER_PIN, INPUT);
  digitalWrite(GPS_SLEEP_PIN, HIGH); // GPS on for default.
  digitalWrite(LED, LOW);
  digitalWrite(ERR_LED, LOW);

#ifdef DEVMODE
  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB)
    ;
  SerialUSB.println("StratoSoar MK3.0 Autopilot.");
#endif

  imuSetup();

#ifdef USE_GPS
  gpsSetup();
#endif
#ifndef USE_GPS
  lat = TESTING_LAT, lon = TESTING_LON;
#endif
}

void loop() {
#ifdef USE_GPS
  lat, lon, altitude, year, month, day, hour, minute, second = getGPSData();
#endif
  turnAngle = turningAngle(lat, lon, yaw, TARGET_LAT, TARGET_LON);
  servoPositionLeft, servoPositionRight = pidElevons(pitch, yaw, turnAngle);
  moveLeftServo(servoPositionLeft);
  moveRightServo(servoPositionRight);
}
