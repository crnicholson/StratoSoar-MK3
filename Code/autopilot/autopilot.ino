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
// - Add correct flying wing dynamics
// - Test bi directional communication
// - Compile with different enables
// - Add a minimum temeperature for the cutdown mechanism
// - Add a flare on landing
// - Finish the SD card code
// - Take photos at certain areas
// - Add better turning with beizer for the waypoints

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
#include "waypoint.h"
#include <Scheduler.h>

// Below are global variables. If you edit these, you'll need to edit vars.h as well.

// GPS vars.
float lat, lon, altitude, targetLat = TARGET_LAT, targetLon = TARGET_LON, hdop;
int year, month, day, hour, minute, second, gpsLast;

// Navigation/IMU vars.
int turnAngle, servoPositionLeft, servoPositionRight, servoPositionRudder, servoPositionElevator, distance;
float yaw, pitch, roll;

// Environmental vars.
float temperature, pressure, bmeAltitude;
int humidity;

// Other vars.
int lastLoRa, loraUpdateRate = LORA_UPDATE_RATE, updateRate = UPDATE_RATE, lastUpdate;
bool abortFlight, altitudeLock, lowVoltage, ultraLowVoltage;
float desiredPitch = DESIRED_PITCH, voltage;

struct data toGliderStruct;

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

  longBlink(LED); // Show successful serial init.

  Wire.begin();
  imuSetup();
  servoSetup();
#ifdef USE_EEPROM
  eepromSetup();
#endif
#ifdef USE_LORA
  loraSetup();
#endif
#ifdef USE_BME
  if (findDevice(0x77) || findDevice(0x76)) {
#ifdef DEVMODE
    SerialUSB.println("BME280 has been found.");
#endif
    if (findDevice(0x77))
      BME280setI2Caddress(119);
    if (findDevice(0x76))
      BME280setI2Caddress(118);
  } else {
#ifdef DEVMODE
    SerialUSB.println("BME280 has not been found. Freezing sketch.");
#endif
    while (1)
      longBlink(ERR_LED);
  }
#endif
#ifdef USE_GPS
#ifdef DEVMODE
  SerialUSB.println("Setting up GPS and waiting for a fix. This may take a while.");
#endif
  gpsSetup();
  waitForFix();
#endif
#ifdef USE_WAYPOINTS
  waypointsSetup();
#endif

#ifndef USE_GPS
  lat = TESTING_LAT, lon = TESTING_LON;
#endif

#ifdef DEVMODE
  SerialUSB.println("Testing servos...");
#endif
#ifdef FLYING_WING
  moveLeftServo(90);
  moveRightServo(90);
  moveLeftServo(0);
  moveRightServo(0);
  moveLeftServo(90);
  moveRightServo(90);
#endif
#ifndef FLYING_WING
  moveRudder(90);
  moveElevator(90);
  moveRudder(0);
  moveElevator(0);
  moveRudder(90);
  moveElevator(90);
#endif

#ifdef FIND_PITCH
#ifndef USE_BME
#error "Please enable BME to find the optimum pitch."
#ifdef DEVMODE
  SerialUSB.println("Please enable BME to find the optimum pitch.");
#endif
  while (1) {
    longBlink(ERR_LED);
  }
#endif
#ifndef USE_GPS
#error "Please enable GPS to find the optimum pitch."
#ifdef DEVMODE
  SerialUSB.println("Please enable GPS to find the optimum pitch.");
#endif
  while (1) {
    longBlink(ERR_LED);
  }
#endif
#ifdef DEVMODE
  SerialUSB.print("Starting the optimum pitch finder program. Please drop the glider for ");
  SerialUSB.print(PITCH_RANGE * PITCH_STEPS * STEP_TIME);
  SerialUSB.println(" plus an additional 15 seconds for the glider to get up to speed.");
#endif
  desiredPitch = findBestPitch();
#endif

  Scheduler.startLoop(loop1);
#ifdef USE_GPS
  Scheduler.startLoop(loop2);
#endif
#ifdef USE_LORA
  Scheduler.startLoop(loop3);
#endif

#ifdef DEVMODE
  SerialUSB.println("Everything has initialized, moving on to main sketch.");
#endif
}

void loop() { // We're doing threading on an Arduino! (This is a really cool library).
  imuMath();  // This runs in the background 24/7. When the code is ready to access the data, getAHRS() is called.
  yield();
}

void loop1() {
#ifdef DROP_START
  int altitudeCounter = 0;
  float lastAltitude = 0;
  while (bmeAltitude < LOCK_ALTITUDE) {
    getBMEData(SEA_LEVEL_PRESSURE); // Get BME data, put into global variables.
#ifdef DEVMODE
    SerialUSB.println("Waiting to initiate flight until the lock altitude is reached.");
#endif
    delay(1000);
  }
  while (altitudeCounter < 2) {
    lastAltitude = bmeAltitude;
    getBMEData(SEA_LEVEL_PRESSURE); // Get BME data, put into global variables.
    if (bmeAltitude < lastAltitude) {
      altitudeCounter++;
    }
#ifdef DEVMODE
    SerialUSB.println("Waiting until the altitude is dropping for two times in a row.");
#endif
    delay(1000);
  }
  moveLeftServo(90);
  moveRightServo(90);
  delay(1000);
  moveLeftServo(95);
  moveRightServo(95);
  delay(5000); // Give enough time for the glider to stabilize in flight before starting steering shenanigans.
#endif

#ifdef SERVO_NONBLOCKING
#ifdef FLYING_WING
  updateLeftServo();
  updateRightServo();
#endif
#ifndef FLYING_WING
  updateRudder();
  updateElevator();
#endif
#endif

  if (millis() - lastUpdate > updateRate) {
    getAHRS(); // Get the yaw, pitch, and roll from the IMU and assign it the respective global variables.

    // Getting sensor values.
#ifdef USE_BME
    getBMEData(SEA_LEVEL_PRESSURE); // Get BME data, put into global variables.
#endif

    // Calculations.
    distance = calculateDistance(lat, lon, targetLat, targetLon);
    turnAngle = turningAngle(lat, lon, yaw, targetLat, targetLon);

#ifdef USE_VOLTAGE
    voltage = readVoltage();
#endif
#ifndef USE_VOLTAGE
    voltage = (LOW_VOLTAGE + 0.1) * 2; // Fake the voltage if reader is disabled.
#endif
    if (voltage < LOW_VOLTAGE) {
      lowVoltage = true;
      updateRate = 5000;      // Move the servos less frequently.
      loraUpdateRate = 30000; // Reduce the LoRa update rate.
    }
    if (voltage < TOO_LOW_VOLTAGE) {
      ultraLowVoltage = true;
      loraUpdateRate = 60000; // Reduce the LoRa update rate even further.
      abortFlight = true;     // Land the glider.
    }

#ifdef USE_WAYPOINTS
    if (!lowVoltage) {
      updateWaypoint(); // Update the target lat and lon if waypoints are enabled.
    }
#endif

    // Landing.
    if (altitude > LOCK_ALTITUDE) {
      altitudeLock = true; // This enables when the glider is above a certain altitude. This makes sure it doesn't land when we're releasing it.
    }
    if (altitude < LAND_ALTITUDE + TARGET_ALT && altitudeLock) {
      abortFlight = true;
    }
    if (abortFlight) {
      getNextCircleWaypoint(lat, lon, 30); // Experimental. Get new targetLat and targetLon which sends glider into a spiral.
    }

    // Calculate again if needed.
    if (abortFlight) {
      distance = calculateDistance(lat, lon, targetLat, targetLon);
      turnAngle = turningAngle(lat, lon, yaw, targetLat, targetLon);
    }

#ifdef FLYING_WING
    pidElevons(pitch, turnAngle); // Get servoPositionLeft, servoPositionRight.
#ifdef SERVO_NONBLOCKING
    startLeftServo(servoPositionLeft);
    startRightServo(servoPositionRight);
#endif
#ifndef SERVO_NONBLOCKING
    moveLeftServo(servoPositionLeft);
    moveRightServo(servoPositionRight);
#endif
#endif
#ifndef FLYING_WING
    servoPositionRudder = pidRudder(turnAngle);
    servoPositionElevator = pidElevator(pitch);
#ifdef SERVO_NONBLOCKING
    startRudder(servoPositionRudder);
    startElevator(servoPositionElevator);
#endif
#ifndef SERVO_NONBLOCKING
    moveRudder(servoPositionRudder);
    moveElevator(servoPositionElevator);
#endif
#endif

    // EEPROM.
#ifdef USE_EEPROM
    writeDataToEEPROM(lat, lon, altitude, yaw, pitch, roll, hour, minute, second); // Write all the data to EEPROM.
#endif

#ifdef DEVMODE
#ifdef DISPLAY_DATA
    printData(); // Print all the data to the serial monitor.
#endif
#endif

    lastUpdate = millis();
  }
  yield();
}

#ifdef USE_GPS
void loop2() {
  // If not using GPS low power, just get data and do nothing else.
#ifndef GPS_LOW_POWER
  if (millis() - gpsLast > GPS_UPDATE_RATE) {
    gpsLast = millis();
    getGPSData(); // Get GPS data. If data is invalid, it waits for a better fix.
  }
#endif
  // If using GPS low power mode, get data and put GPS to sleep.
#ifdef GPS_LOW_POWER
  if (millis() - gpsLast > GPS_LOW_POWER_RATE) {
    gpsLast = millis();
    gpsWakeup();  // Wakeup GPS module and wait for fix.
    getGPSData(); // Get GPS data and put into global variables.
    gpsSleep();   // Put GPS module to sleep.
  }
#endif
  yield();
}
#endif

#ifdef USE_LORA
void loop3() {
#ifdef FAST_LORA
  toGliderStruct.lat = lat;
  toGliderStruct.lon = lon;
  toGliderStruct.tLat = targetLat;
  toGliderStruct.tLon = targetLon;
  toGliderStruct.altitude = altitude;
  toGliderStruct.temperature = temperature;
  toGliderStruct.pressure = pressure;
  toGliderStruct.humidity = humidity;
  toGliderStruct.volts = voltage;
  toGliderStruct.yaw = yaw;
  toGliderStruct.pitch = pitch;
  toGliderStruct.roll = roll;
  toGliderStruct.year = year;
  toGliderStruct.month = month;
  toGliderStruct.day = day;
  toGliderStruct.hour = hour;
  toGliderStruct.minute = minute;
  toGliderStruct.second = second;
  toGliderStruct.txCount++;
  toGliderStruct.abortFlight = abortFlight;
  sendData(toGliderStruct);
  lastLoRa = millis();
#endif
#ifndef FAST_LORA
  if (millis() - lastLoRa > loraUpdateRate) {
    toGliderStruct.lat = lat;
    toGliderStruct.lon = lon;
    toGliderStruct.tLat = targetLat;
    toGliderStruct.tLon = targetLon;
    toGliderStruct.altitude = altitude;
    toGliderStruct.temperature = temperature;
    toGliderStruct.pressure = pressure;
    toGliderStruct.humidity = humidity;
    toGliderStruct.volts = voltage;
    toGliderStruct.yaw = yaw;
    toGliderStruct.pitch = pitch;
    toGliderStruct.roll = roll;
    toGliderStruct.year = year;
    toGliderStruct.month = month;
    toGliderStruct.day = day;
    toGliderStruct.hour = hour;
    toGliderStruct.minute = minute;
    toGliderStruct.second = second;
    toGliderStruct.txCount++;
    toGliderStruct.abortFlight = abortFlight;
#ifdef HAMMING
    sendHammingData(toGliderStruct); // Sends with forward error correction for redundancy over long distances.
#endif
#ifndef HAMMING
    normalSend(); // Sends without forward error correction.
#endif
    lastLoRa = millis();
#endif
  }
#ifdef HAMMING
  hammingReceive(); // Receive LoRa data to abort flight or change targetLat and targetLon.
#endif
#ifndef HAMMING
  normalReceive(); // Receive LoRa data to abort flight or change targetLat and targetLon.
#endif
}
#endif