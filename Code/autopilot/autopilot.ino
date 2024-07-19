/*
autopilot.ino, part of StratoSoar MK3, for an autonomous glider.
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

// To-do:
// - Add LoRa reciever code
// - Add correct flying wing dynamics
// - Use threading for the imu
// - Add a fast update period
// - Find a way to land
// - Add two way communications for changing landing location

#include "blink.h"
#include "bme280.h"
#include "calc.h"
#include "eeprom.h"
#include "gps.h"
#include "i2c_scan.h"
#include "imu.h"
#include "lora.h"
#include "misc.h"
#include "servo.h"
#include "settings.h"
#include "vars.h"

ICM20948 imu;

// GPS vars.
float lat, lon, altitude;
int year, month, day, hour, minute, second, gpsLast;

// Navigation/IMU vars.
int yaw, pitch, roll, turnAngle, servoPositionLeft, servoPositionRight, distance;

// Environmental vars.
float temperature, pressure, bmeAltitude;
int humidity;

int lastLoRa;

struct data packet;

void setup() {
  pinMode(GPS_SLEEP_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ERR_LED, OUTPUT);
  pinMode(LEFT_FET, OUTPUT);
  pinMode(RIGHT_FET, OUTPUT);
  pinMode(LORA_RESET_PIN, OUTPUT);
  pinMode(DIO0_PIN, OUTPUT);
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
#ifdef USE_EEPROM
  eepromSetup();
#endif
#ifdef USE_LORA
  loraSetup();
#endif
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
  SerialUSB.println("Everything has initialized, moving on to main sketch in 5 seconds.");
#endif
  delay(5000);
}

void loop() {
  imuMath();
  if (imu.delt_t > UPDATE_RATE) {
#ifdef USE_GPS
#ifndef GPS_LOW_POWER
    if (millis() - gpsLast > GPS_UPDATE_RATE) {
      gpsLast = millis();
      lat, lon, altitude, year, month, day, hour, minute, second = getGPSData();
    }
#endif
#ifdef GPS_LOW_POWER
    if (millis() - gpsLast > GPS_LOW_POWER_RATE) {
      gpsLast = millis();
      gpsWakeup(); // Wakeup GPS module and wait for fix.
      lat, lon, altitude, year, month, day, hour, minute, second = getGPSData();
      gpsSleep(); // Put GPS module to sleep.
    }
#endif
#endif
#ifdef USE_BME
    temperature, humidity, pressure, bmeAltitude = getBMEData(SEA_LEVEL_PRESSURE);
#endif
    distance = calculateDistance(lat, lon, TARGET_LAT, TARGET_LON);
    turnAngle = turningAngle(lat, lon, yaw, TARGET_LAT, TARGET_LON);
    servoPositionLeft, servoPositionRight = pidElevons(pitch, yaw, turnAngle);
    moveLeftServo(servoPositionLeft);
    moveRightServo(servoPositionRight);
#ifdef USE_EEPROM
    writeDataToEEPROM(lat, lon, altitude, yaw, pitch, roll, hour, minute, second); // Write all the data to EEPROM.
#endif
#ifdef USE_LORA
    if (millis() - lastLoRa > LORA_UPDATE_RATE) {
      packet.lat = lat;
      packet.lon = lon;
      packet.tLat = TARGET_LAT;
      packet.tLon = TARGET_LON;
      packet.altitude = altitude;
      packet.temperature = temperature;
      packet.pressure = pressure;
      packet.humidity = humidity;
      packet.yaw = yaw;
      packet.pitch = pitch;
      packet.roll = roll;
      packet.hour = hour;
      packet.minute = minute;
      packet.second = second;
      packet.txCount++;
      sendData(packet);
      lastLoRa = millis();
#endif
    }
    imu.count = millis();
  }
}
