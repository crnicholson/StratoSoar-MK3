#include "blink.h"
#include "calc.h"
#include "gps.h"
#include "i2c_scan.h"
#include "imu.h"
#include "misc.h"
#include "servo.h"
#include "settings.h"
#include "vars.h"

// GPS vars.
float lat, lon, altitude;
int year, month, day, hour, minute, second;
long gpsLast;

// Navigation/IMU vars.
int yaw, pitch, roll, turnAngle, servoPositionLeft, servoPositionRight;
long distance;

void setup() {
  pinMode(GPS_SLEEP_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ERR_LED, OUTPUT);
  pinMode(LEFT_FET, OUTPUT);
  pinMode(RIGHT_FET, OUTPUT);
  pinMode(VOLTMETER_PIN, INPUT);
  digitalWrite(GPS_SLEEP_PIN, HIGH); // GPS on for default.
  digitalWrite(LED, LOW);
  digitalWrite(ERR_LED, LOW);
  digitalWrite(LEFT_FET, LOW);  // Servo off for default.
  digitalWrite(RIGHT_FET, LOW); // Servo off for default.

  longBlink(LED);     // Show startup.
  longBlink(ERR_LED); // Show startup.

#ifdef DEVMODE
  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB)
    longBlink(ERR_LED);
  SerialUSB.println("StratoSoar MK3.0 Autopilot.");
#endif

  longBlink(LED); // Show successful Serial init.

  Wire.begin();
  imuSetup();
  servoSetup();

#ifdef USE_GPS
#ifdef DEVMODE
  SerialUSB.println("Setting up GPS and waiting for a fix. This may take a while.");
#endif
  gpsSetup();
  waitForFix();
#endif
#ifndef USE_GPS
  lat = TESTING_LAT, lon = TESTING_LON;
#endif

#ifdef DEVMODE
  SerialUSB.println("Testing servos...");
#endif
  moveLeftServo(90);
  moveRightServo(90);
  moveLeftServo(0);
  moveRightServo(0);
  moveLeftServo(90);
  moveRightServo(90);
#ifdef DEVMODE
  SerialUSB.println("Everything has initialized, moving on to main sketch.");
#endif
}

void loop() {
#ifndef GPS_LOW_POWER
  if (millis() - gpsLast > GPS_UPDATE_RATE) {
    gpsLast = millis();
#ifdef USE_GPS
    lat, lon, altitude, year, month, day, hour, minute, second = getGPSData();
#endif
  }
#endif
#ifdef GPS_LOW_POWER
  if (millis() - gpsLast > GPS_LOW_POWER_RATE) {
    gpsLast = millis();
    gpsWakeup(); // Wakeup GPS module and wait for fix.
#ifdef USE_GPS
    lat, lon, altitude, year, month, day, hour, minute, second = getGPSData();
#endif
    gpsSleep(); // Put GPS module to sleep.
  }
#endif
  distance = calculateDistance(lat, lon, TARGET_LAT, TARGET_LON);
  turnAngle = turningAngle(lat, lon, yaw, TARGET_LAT, TARGET_LON);
  servoPositionLeft, servoPositionRight = pidElevons(pitch, yaw, turnAngle);
  moveLeftServo(servoPositionLeft);
  moveRightServo(servoPositionRight);
}
