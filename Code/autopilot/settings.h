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
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// Enables.
#define DEVMODE // Toggle serial monitor.
#define USE_GPS
#define GPS_LOW_POWER // Sets the GPS to sleep after waking up after every GPS_LOW_POWER_RATE milliseconds.
#define USE_BME
#define USE_EEPROM

// Pins.
#define LED 13
#define ERR_LED 12
#define GPS_SLEEP_PIN 11
#define LEFT_SERVO 10
#define RIGHT_SERVO 9
#define LEFT_FET 8
#define RIGHT_FET 7
#define VOLTMETER_PIN A0

// Other.
#define UPDATE_RATE 1000           // The time in milliseconds between glider respositionings.
#define BAUD_RATE 115200           // Baud rate of the serial monitor.
#define SEA_LEVEL_PRESSURE 1013.25 // Sea level pressure in hPa.

// EEPROM.
// #define ERASE_EEPROM // Erases the EEPROM every start. Highly unreccomended.
#define EEPROM_SIZE 512 // Valid types: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1025, 2048

// GPS.
#define GPS_BAUD_RATE 9600       // Baud rate of the GPS module.
#define GPS_UPDATE_RATE 5000     // The time in milliseconds stating the frequency of new GPS data.
#define GPS_LOW_POWER_RATE 15000 // The time in milliseconds stating the frequency of new GPS data in low power mode.

// Coordinates.
#define TARGET_LAT 41.40338  // Target latitude.
#define TARGET_LON -71.9533  // Target longitude.
#define TESTING_LAT 41.30338 // Testing latitude.
#define TESTING_LON -71.8533 // Testing longitude.

#define PI 3.1415926
