// #include "sd.h"

// // To-do:
// // - Power cycle the SD card.
// // - Use the EEPROM as a buffer for the SD card.
// // - Add unique file names for each flight.

// #define SD_CS 10
// #define FILE_NAME "data.csv"

// void sdSetup() {
//   if (!SD.begin(SD_CS)) {
// #ifdef DEVMODE
//     SerialUSB.println("SD card initialization failed!");
// #endif
//     while (1) {
//       longBlink(ERR_LED);
//     }
//   }
// #ifdef DEVMODE
//   SerialUSB.println("SD card initialized.");
// #endif

//   // Check if the file exists, if not, create it and add headers.
//   if (!SD.exists(FILE_NAME)) {
//     File dataFile = SD.open(FILE_NAME, FILE_WRITE);
//     if (dataFile) {
//       dataFile.println("Latitude,Longitude,Target Latitude,Target Longitude,Altitude,Temperature,Pressure,Humidity,Volts,Yaw,Pitch,Roll,Hour,Minute,Second,Transmission Count,Abort Flight");
//       dataFile.close();
// #ifdef DEVMODE
//       SerialUSB.println("File created and headers written.");
// #endif
//     } else {
// #ifdef DEVMODE
//       SerialUSB.println("Error creating file.");
// #endif
//       while (1) {
//         longBlink(ERR_LED);
//       }
//     }
//     else {
//       SerialUSB.println("File already exists.");
//     }
//   }
// }

// void writeDataToSD(Packet toGliderStruct) {
//   File dataFile = SD.open(FILE_NAME, FILE_WRITE);

//   if (dataFile) {
//     dataFile.print(toGliderStruct.lat);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.lon);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.tLat);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.tLon);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.altitude);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.temperature);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.pressure);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.humidity);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.volts);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.yaw);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.pitch);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.roll);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.hour);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.minute);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.second);
//     dataFile.print(",");
//     dataFile.print(toGliderStruct.txCount);
//     dataFile.print(",");
//     dataFile.println(toGliderStruct.abortFlight);

//     dataFile.close();
//   } else {
// #ifdef DEVMODE
//     SerialUSB.println("Error opening data.csv");
// #endif
//   }
// }