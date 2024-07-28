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
#define DEVMODE       // Toggle serial monitor.
#define GPS_LOW_POWER // Sets the GPS to sleep after waking up after every GPS_LOW_POWER_RATE milliseconds.
#define USE_GPS
#define USE_BME
#define USE_EEPROM
#define USE_LORA
#define FAST_LORA // Ignore other LoRa settings and use a very quick LoRa update rate for short range.
#define WAYPOINTS

// Pins.
#define LED 13
#define ERR_LED 12
#define GPS_SLEEP_PIN 11
#define LEFT_SERVO 10
#define RIGHT_SERVO 9
#define LEFT_FET 8
#define RIGHT_FET 7
#define LORA_RESET_PIN 6
#define DIO0_PIN 5
// #define BUTTON 4   // Reserved for future use.
// #define RESERVED 1 // Reserved for Serial.
// #define RESERVED 0 // Reserved for Serial.
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
#define DECLINATION 14             // http://www.ngdc.noaa.gov/geomag-web/#declination
#define LAND_ALTITUDE 200          // Under this many meters in altitude, the glider will start it's landing sequence.
#define LOCK_ALTITUDE 1000         // The landing sequence will only be available after this many meters in altitude.

// EEPROM.
// #define ERASE_EEPROM // Erases the EEPROM every start. Highly unreccomended.
#define EEPROM_SIZE 512    // Valid types: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1025, 2048
#define BYTES_PER_CYCLE 19 // Number of bytes written to the EEPROM per cycle.
#define EEPROM_BUFFER 50   // Buffer in milliseconds for the frequency of EEPROM write cycles.
#define FLIGHT_TIME 180    // The estimated time in minutes of the flight, used in estimating the write cycle of the EEPROM.

// GPS.
#define GPS_BAUD_RATE 9600       // Baud rate of the GPS module.
#define GPS_UPDATE_RATE 5000     // The time in milliseconds stating the frequency of new GPS data.
#define GPS_LOW_POWER_RATE 15000 // The time in milliseconds stating the frequency of new GPS data in low power mode.

// Coordinates.
#define TARGET_LAT 41.40338  // Target latitude, this could get changed by the ground station.
#define TARGET_LON -71.9533  // Target longitude, this could get changed by the ground station.
#define TESTING_LAT 41.30338 // Testing latitude.
#define TESTING_LON -71.8533 // Testing longitude.

// LoRa.
#define FREQUENCY 433E6       // Frequency of your LoRa module.
#define SPREADING_FACTOR 10   // Choose this based on this https://forum.arduino.cc/t/what-are-the-best-settings-for-a-lora-radio/449528.
#define BANDWIDTH 62.5E3      // Choose this based on this https://forum.arduino.cc/t/what-are-the-best-settings-for-a-lora-radio/449528.
#define SYNC_WORD 0xFB        // Only other devices with this sync word can receive your broadcast.
#define CALL_SIGN "XXXXXX"    // Your call sign - make sure to change this. Uneeded in the US if using 915 MHz.
#define LORA_UPDATE_RATE 5000 // The milliseconds between LoRa updates.

#define PI 3.1415926
