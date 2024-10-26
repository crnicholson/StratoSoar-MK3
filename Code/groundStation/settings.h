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

// Enables.
#define DEVMODE   // Toggle serial monitor.
#define FAST_LORA // Ignore other LoRA settings and use a fast LoRa setup.
#define USE_SD    // Toggle the use of an SD card for data logging.

// Pins.
#define LED 3 // SCK uses pin 13.
#define SS_PIN 10
#define RESET_PIN 9
#define DIO0_PIN 8

#define BAUD_RATE 115200 // Baud rate of the serial monitor.

// Uploader location.
#define U_LAT 42.00
#define U_LON -71.00
#define U_ALT 1000
