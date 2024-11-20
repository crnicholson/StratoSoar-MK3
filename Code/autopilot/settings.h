/*
settings.h, part of StratoSoar MK3, for an autonomous glider.
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
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

// Enables.
#define DEVMODE // Toggle serial monitor.
// #define GPS_LOW_POWER // Sets the GPS to sleep after waking up after every GPS_LOW_POWER_RATE milliseconds.
// #define USE_GPS
// #define USE_BME
// #define USE_EEPROM
// #define USE_LORA
// #define USE_VOLTAGE
#define FAST_LORA // Ignore other LoRa settings and use a very quick LoRa update rate for short range.
// #define USE_WAYPOINTS
// #define SMOOTH_TURNING // Smoothly turn the glider towards the next waypoint.
// #define FIND_PITCH // Use a program to find the best pitch of the glider. BME and GPS must be enabled. Very experimental.
// #define DROP_START // Don't start the program until the glider has detected a drop in altitude.
#define DISPLAY_DATA // Dump the data on the serial monitor every update.
// #define FLYING_WING  // If enabled, the glider uses flying wing kinematics. If not, it uses the traditional rudder and elevator.
// #define SERVO_NONBLOCKING // Use non-blocking code for the servo.
// #define HAMMING // Use hamming encoding for the LoRa data.

// Pins.
#define LED 13
#define ERR_LED 12
#define GPS_SLEEP_PIN 11
#define LEFT_SERVO 10
#define RUDDER_PIN 10
#define RIGHT_SERVO 9
#define ELEVATOR_PIN 9
#define LEFT_FET 8
#define RUDDER_FET 8
#define RIGHT_FET 7
#define ELEVATOR_FET 7
#define LORA_RESET_PIN 6
#define DIO0_PIN 5
// #define BUTTON 4   // Reserved for future use.
// #define RESERVED 1 // Reserved for serial GPS connection.
// #define RESERVED 0 // Reserved for serial GPS connection.
#define SS_PIN 38
#define VOLTMETER_PIN A0

// Pins broken out.
// #define SDA     // Physical pin 31
// #define SCL     // Physical pin 32
// #define MOSI    // Physical pin 19
// #define MISO    // Physical pin 21
// #define SCK     // Physical pin 20
// #define GPIO 3  // Physical pin 14
// #define GPIO 2  // Physical pin 23
// #define GPIO A1 // Physical pin 7

// Other.
#define UPDATE_RATE 1000           // The time in milliseconds between glider respositionings.
#define BAUD_RATE 115200           // Baud rate of the serial monitor.
#define SEA_LEVEL_PRESSURE 1013.25 // Sea level pressure in hPa.
#define DECLINATION 0              // -13.5          // Local magnetic declination in degrees in Boston. Yaw increases CW from North. Can be found here: https://www.ngdc.noaa.gov/geomag-web/#declination.
#define AD0_VAL 0                  // Change I2C address of IMU. 0 = 0x68, 1 = 0x69.
#define LAND_ALTITUDE 100          // This many meters from the gorund the glider will start its landing sequence.
#define LOCK_ALTITUDE 1000         // The landing sequence will only be available after this many meters in altitude.
#define CHANGE_WAYPOINT 10         // The distance in meters between the current location and the waypoint needed before changing to the next waypoint.
#define LOW_VOLTAGE 3.2            // Below this voltage the glider will reduce the frequency of position updates and servo updates.
#define TOO_LOW_VOLTAGE 3.0        // Below this voltage the glider will simply spiral down with even fewer updates.

// Glider pitch control.
#define DESIRED_PITCH -5.0 // Pitch the glider should be flying at during the flight.
#define STARTING_PITCH -6  // The pitch the glider starts at before finding the best pitch.
#define PITCH_RANGE 4      // This many degrees will be added to the value of PITCH as the high value.
#define PITCH_STEPS 1      // This is the number of degrees stepped upwards when finding the optimum pitch angle per cycle.
#define STEP_TIME 10       // Time in seconds spent on each step.

#define SETPOINT_PITCH 0    // For the flying wing.
#define SETPOINT_ELEVATOR 0 // For the traditional setup.

// PID settings.
#define KP_LEFT 1
#define KI_LEFT 0
#define KD_LEFT 0

#define KP_RIGHT 1
#define KI_RIGHT 0
#define KD_RIGHT 0

#define KP_RUDDER 1
#define KI_RUDDER 0
#define KD_RUDDER 0

#define KP_ELEVATOR 1
#define KI_ELEVATOR 0
#define KD_ELEVATOR 0

// EEPROM.
// #define ERASE_EEPROM // Erases the EEPROM every start. Highly unreccomended.
#define EEPROM_SIZE 512    // Valid types: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1025, 2048
#define BYTES_PER_CYCLE 19 // Number of bytes written to the EEPROM per cycle.
#define EEPROM_BUFFER 50   // Buffer in milliseconds for the frequency of EEPROM write cycles.
#define FLIGHT_TIME 180    // The estimated time in minutes of the flight, used in estimating the write cycle of the EEPROM.

// GPS.
#define GPS_BAUD_RATE 9600       // Baud rate of the GPS module.
#define GPS_UPDATE_RATE 1000     // The time in milliseconds stating the frequency of new GPS data. Cannot go lower than 1000 ms without chaning config on GPS.
#define GPS_LOW_POWER_RATE 15000 // The time in milliseconds stating the frequency of new GPS data in low power mode.

// Coordinates. Coordinates for the waypoint system are set in "waypoint.cpp".
#define TARGET_LAT 41.40338  // Target latitude, this could get changed by the ground station.
#define TARGET_LON -71.9533  // Target longitude, this could get changed by the ground station.
#define TARGET_ALT 100       // Altitude of the target location.
#define TESTING_LAT 41.30338 // Testing latitude.
#define TESTING_LON -71.8533 // Testing longitude.

// LoRa.
#define FREQUENCY 433E6       // Frequency of your LoRa module.
#define SPREADING_FACTOR 10   // Choose this based on this https://forum.arduino.cc/t/what-are-the-best-settings-for-a-lora-radio/449528.
#define BANDWIDTH 62.5E3      // Choose this based on this https://forum.arduino.cc/t/what-are-the-best-settings-for-a-lora-radio/449528.
#define SYNC_WORD 0xFB        // Only other devices with this sync word can receive your broadcast.
#define CALL_SIGN "XXXXXX"    // Your call sign - make sure to change this. Uneeded in the US if using 915 MHz.
#define LORA_UPDATE_RATE 5000 // The milliseconds between LoRa updates.

// These are the previously determined offsets and scale factors for the accelerometer and magnetometer.
// Gyro default scale 250 dps. Convert to radians/sec subtract offsets
static float G_offset[3]{69.6, 22.0, -35.1};

static float A_B[3]{774.57, 109.87, 821.08};

static float A_Ainv[3][3]{{0.0602, -0.00097, -0.00513},
                          {-0.00097, 0.06405, -0.00172},
                          {-0.00513, -0.00172, 0.0561}};

static float M_B[3]{15.29, 184.51, 37.83};

static float M_Ainv[3][3]{{3.03796, -0.04154, -0.0047},
                          {-0.04154, 2.95127, -0.02467},
                          {-0.0047, -0.02467, 2.95456}};