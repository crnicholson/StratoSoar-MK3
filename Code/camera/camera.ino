#include <Arduino_OV767X.h>
#include <SD.h>
#include <SPI.h>

// To-do:
// - Add control from other sketches
// - Add low power mode

File videoFile;

#define CS 5   // CS pin for SD card.
#define LED 13 // Pin where LED is attached.

#define DEVMODE // Toggle serial monitor.

#define BAUD_RATE 115200 // Baud rate of the serial monitor.

// Circuit, from what I found online:
//     - OV7670 camera module:
//       - 3.3 connected to 3.3
//       - GND connected GND
//       - SIOC connected to A5
//       - SIOD connected to A4
//       - VSYNC connected to 8
//       - HREF connected to A1
//       - PCLK connected to A0
//       - XCLK connected to 9
//       - D7 connected to 4
//       - D6 connected to 6
//       - D5 connected to 5
//       - D4 connected to 3
//       - D3 connected to 2
//       - D2 connected to 0 / RX
//       - D1 connected to 1 / TX
//       - D0 connected to 10

void setup() {
#ifdef DEVMODE
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    longBlink(LED);
  }

  Serial.println("OV7670 Camera Capture to SD Card.");
#endif

  if (!SD.begin(CS)) {
#ifdef DEVMODE
    Serial.println("SD card initialization failed!");
#endif
    while (1) {
      longBlink(LED);
    }
  }

  if (!Camera.begin(CIF, RGB565, 1)) {
#ifdef DEVMODE
    Serial.println("Failed to initialize camera!");
#endif
    while (1) {
      longBlink(LED);
    }
  }

#ifdef DEVMODE
  Serial.println("Camera settings:");
  Serial.print("\twidth = ");
  Serial.println(Camera.width());
  Serial.print("\theight = ");
  Serial.println(Camera.height());
  Serial.print("\tbits per pixel = ");
  Serial.println(Camera.bitsPerPixel());
#endif
}

void loop() {
  videoFile = SD.open("/video.bin", FILE_WRITE); // Create a new file on the SD card.

  if (!videoFile) {
#ifdef DEVMODE
    Serial.println("Failed to open file for writing!");
#endif
    while (1) {
      longBlink(LED);
    }
  }

#ifdef DEVMODE
  Serial.println("Capturing video...");
#endif

  for (int frame = 0; frame < 450; frame++) { // Capture 15 seconds of video at 15 fps.
    Camera.readFrame();
    int numPixels = Camera.width() * Camera.height();

    for (int i = 0; i < numPixels; i++) {
      unsigned short p = Camera.readPixel(); // Read pixel directly from camera buffer.
      videoFile.write((uint8_t *)&p, 2);     // Write the pixel data to the SD card.
    }
  }

  videoFile.close();
#ifdef DEVMODE
  Serial.println("Video capture complete.");
#endif
}
