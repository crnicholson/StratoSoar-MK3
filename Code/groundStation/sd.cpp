/*
sd.cpp part of StratoSoar MK3, for an autonomous glider.
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

#include "sd.h"

void sdSetup() {
  // Initialize the SD card.
  if (!SD.begin(SD_SS_PIN)) {
#ifdef DEVMODE
    Serial.println("SD card initialization failed! Freezing sketch.");
#endif
    while (1)
      longBlink(LED);
  }

#ifdef DEVMODE
  Serial.println("SD card is ready.");
#endif
}

// char getAllFiles() {
//   dir = SD.open("/");

//   int fileCounter = 0;
//   char *files[] = {};

//   while (1) {
//     File entry = dir.openNextFile();

//     if (!entry)
//       return files;

// #ifdef DEVMODE
//     Serial.print(entry.name());
// #endif

//     files[fileCounter] = entry.name();
//     fileCounter++;

//     entry.close();
//   }
// }

void writeToCard() {
  File dataFile = SD.open(String(fromGliderStruct) + ".csv", FILE_WRITE); // Create or open the file.

  dataFile.close();

  File dataFile = SD.open(String(fromGliderStruct) + ".csv");

  if (dataFile.read() != "l") {
    dataFile.close();
    File dataFile = SD.open(String(fromGliderStruct) + ".csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println("lat,lon,altitude,tLat,tLon,temperature,pressure,humidity,volts,yaw,pitch,roll,year,month,day,hour,minute,second,abort,txCount,rxCount,uLat,uLon,uAlt,rssi,snr,callsign");
      dataFile.close();
    } else {
#ifdef DEVMODE
      Serial.println("Error opening file for writing. Freezing sketch.");
#endif
      while (1)
        longBlink(LED);
    }
  }

  File dataFile = SD.open(String(fromGliderStruct) + ".csv", FILE_WRITE);

  if (dataFile) {
    dataFile.print(fromGliderStruct.lat);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.lon);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.altitude);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.tLat);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.tLon);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.temperature);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.pressure);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.humidity);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.volts);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.yaw);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.pitch);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.roll);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.year);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.month);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.day);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.hour);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.minute);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.second);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.abort);
    dataFile.print(",");
    dataFile.print(fromGliderStruct.txCount);
    dataFile.print(",");
    dataFile.print(rxCount);
    dataFile.print(",");
    dataFile.print(U_LAT);
    dataFile.print(",");
    dataFile.print(U_LON);
    dataFile.print(",");
    dataFile.print(U_ALT);
    dataFile.print(",");
    dataFile.print(rssi);
    dataFile.print(",");
    dataFile.print(snr);
    dataFile.print(",");
    dataFile.println(fromGliderStruct.callSign);

    dataFile.close(); // Close the file.
#ifdef DEVMODE
    Serial.println("Data written to SD card.");
#endif
  } else {
#ifdef DEVMODE
    Serial.println("Error opening file for writing. Freezing sketch.");
#endif
    while (1)
      longBlink(LED);
  }
}
