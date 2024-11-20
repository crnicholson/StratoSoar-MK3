/*
vars.h, part of StratoSoar MK3, for an autonomous glider.
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

#include "secrets.h" // I made a secret file with my WiFi and other info.

// Enables.
#define DEVMODE // Toggle serial monitor.
// #define FAST_LORA // Ignore other LoRA settings and use a fast LoRa setup.
// #define USE_SD    // Toggle the use of an SD card for data logging.
// #define HAMMING   // Toggle the use of forward error correction.
#define WIFI // Toggle data sending to SondeHub.

// LoRa settings.
#define FREQUENCY 433E6     // Frequency of your LoRa module.
#define SPREADING_FACTOR 10 // Choose this based on this https://forum.arduino.cc/t/what-are-the-best-settings-for-a-lora-radio/449528.
#define BANDWIDTH 62.5E3    // Choose this based on this https://forum.arduino.cc/t/what-are-the-best-settings-for-a-lora-radio/449528.
#define SYNC_WORD 0xFB      // Only other devices with this sync word can receive your broadcast.

// Pins.
#define LED 13 
#define SS_PIN 2
#define RESET_PIN 12
#define DIO0_PIN 14

#define BAUD_RATE 115200 // Baud rate of the serial monitor.

// Uploader location.
#define U_LAT 42.00
#define U_LON -71.00
#define U_ALT 1000

// WiFi information.
#define YOUR_SSID SECRET_SSID
#define YOUR_PASSWORD SECRET_PASSWORD

#define SERVER_NAME SECRET_SERVER_NAME

#define CALL_SIGN SECRET_CALL_SIGN
