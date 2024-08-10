#include "lora.h"

void loraSetup() {
  LoRa.setPins(SS_PIN, LORA_RESET_PIN, DIO0_PIN);

  if (!LoRa.begin(FREQUENCY)) {
#ifdef DEVMODE
    SerialUSB.println("Starting LoRa failed!");
#endif
    while (1) {
      longBlink(ERR_LED);
    }
  }

#ifndef FAST_LORA
  LoRa.setSyncWord(SYNC_WORD);               // Defined in settings.h.
  LoRa.setSpreadingFactor(SPREADING_FACTOR); // Defined in settings.h.
  LoRa.setSignalBandwidth(BANDWIDTH);        // Defined in settings.h.
  LoRa.crc();                                // Checksum for packet error detection.
#endif

#ifdef FAST_LORA
  LoRa.setSyncWord(SYNC_WORD); // Defined in settings.h.
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(250000);
  LoRa.crc(); // Checksum for packet error detection.
#endif

#ifndef CALL_SIGN
#error "CALL_SIGN not defined in settings.h. Please add one."
#endif
}

void sendData(struct data &newPacket) {
  // Wait until the LoRa is ready to send a new packet!
  while (LoRa.beginPacket() == 0) {
    delay(10);
  }

  LoRa.beginPacket();
  LoRa.write((byte *)&newPacket, sizeof(newPacket));
  LoRa.endPacket(true); // Use async send.
}

void sendHammingData(struct data &newPacket) {
  byte *byteArray = (byte *)&newPacket; // Convert the packet to a byte array.

  byte encodedData[2 * sizeof(newPacket)]; // Hamming(7,4) doubles the size!

  for (size_t i = 0; i < sizeof(newPacket); ++i) {
    byte highNibble = hammingEncode((byteArray[i] >> 4) & 0xF);
    byte lowNibble = hammingEncode(byteArray[i] & 0xF);

    encodedData[2 * i] = highNibble;
    encodedData[2 * i + 1] = lowNibble;
  }

  // Wait until the LoRa is ready to send a new packet!
  while (LoRa.beginPacket() == 0) {
    delay(10);
  }

  LoRa.beginPacket();
  LoRa.write(encodedData, sizeof(encodedData));
  LoRa.endPacket(true); // Use async send.
}

// Function to calculate Hamming code for 4 data bits.
byte hammingEncode(byte data) {
  byte p1 = (data >> 2) & 0x1 ^ (data >> 1) & 0x1 ^ (data >> 0) & 0x1;
  byte p2 = (data >> 3) & 0x1 ^ (data >> 1) & 0x1 ^ (data >> 0) & 0x1;
  byte p3 = (data >> 3) & 0x1 ^ (data >> 2) & 0x1 ^ (data >> 0) & 0x1;

  byte hamming = (p1 << 6) | (p2 << 5) | (p3 << 4) | (data & 0xF);
  return hamming;
}