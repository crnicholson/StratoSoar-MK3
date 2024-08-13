#include <Arduino_OV767X.h>
#include <SD.h>
#include <SPI.h>
#include <esp_sleep.h>

File videoFile;

int fileIndex;

#define CS 7          // CS pin for SD card.
#define LED 13        // Pin where LED is attached.
#define SLEEP_PIN A2  // Pin where the sleep pin is attached.
#define WAKEUP_PIN A3 // Pin where the wakeup pin is attached.

#define DEVMODE // Toggle serial monitor.
#define FOREVER // Toggle infinite recording until the sleep pin is high.

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
  pinMode(LED, OUTPUT);               // Configure LED pin.
  pinMode(SLEEP_PIN, INPUT_PULLDOWN); // Configure sleep pin.

  longBlink(LED);

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
  shortBlink(LED);

  char fileName = "/video" + String(fileInfex) + ".bin";
  videoFile = SD.open(fileName, FILE_WRITE); // Create a new file on the SD card.

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

#ifndef FOREVER
  for (int frame = 0; frame < 450; frame++) { // Capture 15 seconds of video at 15 fps
    Camera.readFrame();
    int numPixels = Camera.width() * Camera.height();

    for (int i = 0; i < numPixels; i++) {
      unsigned short p = Camera.readPixel(); // Read pixel directly from camera buffer
      videoFile.write((uint8_t *)&p, 2);     // Write the pixel data to the SD card
    }
  }
#endif
#ifdef FOREVER
  while (!digitalRead(SLEEP_PIN)) { // Record until sleep is activated!
    Camera.readFrame();
    int numPixels = Camera.width() * Camera.height();

    for (int i = 0; i < numPixels; i++) {
      unsigned short p = Camera.readPixel(); // Read pixel directly from camera buffer
      videoFile.write((uint8_t *)&p, 2);     // Write the pixel data to the SD card
    }
  }
#endif

  videoFile.close();
#ifdef DEVMODE
  Serial.println("Video capture complete, now sleeping.");
#endif
  enterLowPowerMode(); // Don't wakeup until the WAKEUP_PIN is high.
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
  indexFile = SD.open("index.txt");
  if (indexFile) {
    fileIndex = int(indexFile.read());
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
    indexFile.write(fileIndex + 1);
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
  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, HIGH); // Wake up when sleep pin is HIGH.
  esp_deep_sleep_start();
}