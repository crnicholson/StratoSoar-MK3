#include "OV7670.h"
#include <SD.h>
#include <SPI.h>
#include <esp_sleep.h>

#define DEVMODE // Toggle serial monitor.
#define FOREVER // Toggle infinite recording until the sleep pin is high.

#define BAUD_RATE 115200 // Baud rate of the serial monitor.

// Pins.
#define SIOD 8 // SDA.
#define SIOC 9 // SCL.

#define VSYNC 17
#define HREF 18

#define XCLK 47
#define PCLK 48

#define D0 1
#define D1 2
#define D2 3
#define D3 4
#define D4 5
#define D5 6
#define D6 7
#define D7 14

#define SD_CS 10      // Chip select for the SD card.
#define LED 13        // LED pin.
#define SLEEP_PIN 15  // Pin to trigger sleep mode.
#define WAKEUP_PIN 16 // Pin to wake up.

OV7670 *camera;
File videoFile;
int fileIndex = 0;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(SLEEP_PIN, INPUT_PULLDOWN);

  longBlink(LED);

#ifdef DEVMODE
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    longBlink(LED);
  }

  Serial.println("OV7670 Camera Capture to SD Card with Low Power Mode.");
#endif

  // Initialize the camera.
  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);

  if (!SD.begin(SD_CS)) {
#ifdef DEVMODE
    Serial.println("SD card initialization failed!");
#endif
    while (1) {
      longBlink(LED);
    }
  }

  generateFileName(); // Get the current file index and generate a new file name.
}

void loop() {
  shortBlink(LED);

  // Create a new file on the SD card.
  String fileName = "/video" + String(fileIndex) + ".bin";
  videoFile = SD.open(fileName.c_str(), FILE_WRITE);

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

#ifdef FOREVER
  while (!digitalRead(SLEEP_PIN)) { // Record until sleep is activated!
    camera->oneFrame();
    videoFile.write(camera->frame, camera->xres * camera->yres * 2); // Write the frame data to the SD card.
    videoFile.flush();
    delay(67); // Approx. 15 fps.
  }
#endif

#ifndef FOREVER
  for (int frame = 0; frame < 450; frame++) { // Capture 15 seconds of video at 15 fps
    camera->oneFrame();
    videoFile.write(camera->frame, camera->xres * camera->yres * 2); // Write the frame data to the SD card.
    videoFile.flush();
    delay(67); // Approx. 15 fps.
  }
#endif

  videoFile.close();
#ifdef DEVMODE
  Serial.println("Video capture complete, now sleeping.");
#endif

  // Turn off SD card
  // Turn off camera

  enterLowPowerMode(); // Enter low-power mode.
}

void longBlink(int pin) {
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(pin, LOW);
  delay(1000);
}

void shortBlink(int pin) {
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
  delay(100);
}

void generateFileName() {
  File indexFile = SD.open("index.txt", FILE_READ);
  if (indexFile) {
    fileIndex = indexFile.parseInt();
    indexFile.close();
#ifdef DEVMODE
    Serial.print("File index: ");
    Serial.println(fileIndex);
#endif
  } else {
#ifdef DEVMODE
    Serial.println("Error opening the file index file.");
#endif
    while (1) {
      longBlink(LED);
    }
  }

  indexFile = SD.open("index.txt", FILE_WRITE);
  if (indexFile) {
    indexFile.seek(0);
    indexFile.println(fileIndex + 1);
    indexFile.close();
  } else {
#ifdef DEVMODE
    Serial.println("Error opening the file index file.");
#endif
    while (1) {
      longBlink(LED);
    }
  }
}

void enterLowPowerMode() {
#ifdef DEVMODE
  Serial.println("Entering low-power mode...");
#endif
  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, HIGH); // Wake up when WAKEUP_PIN is HIGH.
  esp_deep_sleep_start();
}
