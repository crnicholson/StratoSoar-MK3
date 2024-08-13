#include "OV7670.h"
#include <SD.h>
#include <SPI.h>

#define DEVMODE // Toggle serial monitor.

#define BAUD_RATE 115200 // Baud rate of the serial monitor.

// Pins.
#define SIOD 21 // SDA.
#define SIOC 22 // SCL.

#define VSYNC 34
#define HREF 35

#define XCLK 32
#define PCLK 33

#define D0 27
#define D1 17
#define D2 16
#define D3 15
#define D4 14
#define D5 13
#define D6 12
#define D7 4

#define SD_CS 5 // Chip select for the SD card.

OV7670 *camera;
File videoFile;

unsigned char bmpHeader[BMP::headerSize];

void setup() {
#ifdef DEVMODE
  while (!Serial.begin(BAUD_RATE)) {
    longBlink(LED);
  }
#endif

  // Initialize the camera.
  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  BMP::construct16BitHeader(bmpHeader, camera->xres, camera->yres);

  if (!SD.begin(chipSelect)) {
#ifdef DEVMODE
    Serial.println("Card failed, or not present");
#endif
    while (1) {
      longBlink(LED);
    }
  }
#ifdef DEVMDOE
  Serial.println("SD card initialized.");
#endif

  videoFile = SD.open("/video.bin", FILE_WRITE); // Create a video file.

  if (!videoFile) {
#ifdef DEVMODE
    Serial.println("Failed to create file");
#endif
    while (1) {
      longBlink(LED);
    }
  }
}

void loop() {
  camera->oneFrame();

  videoFile.write(bmpHeader, BMP::headerSize); // Write BMP header and frame data to the SD card.
  videoFile.write(camera->frame, camera->xres * camera->yres * 2);

  videoFile.flush(); // Flush the data to ensure it's written to the SD card.

  delay(67); // Delay to achieve approximately 15 fps (1000ms / 15fps = 66.7ms)

  // videoFile.close(); // End the capture.
}