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

void hammingReceive() {
  int packetSize = LoRa.parsePacket(); // Parse packet.
  if (packetSize > 0) {
    // Ensure the packet size is 18 bytes (9 bytes * 2 after Hamming encoding).
    if (packetSize == 18) {
      while (LoRa.available() >= 18) {
        shortBlink(LED);

        byte encodedData[18];
        LoRa.readBytes(encodedData, sizeof(encodedData));

        byte decodedData[9];

        for (size_t i = 0; i < 9; ++i) {
          byte highNibble = hammingDecode(encodedData[2 * i]);
          byte lowNibble = hammingDecode(encodedData[2 * i + 1]);

          decodedData[i] = (highNibble << 4) | lowNibble;
        }

        memcpy(&targetLat, decodedData, sizeof(float));
        memcpy(&targetLon, decodedData + sizeof(float), sizeof(float));
        abortFlight = decodedData[8];
      }
    } else {
      // Discard the packet if it's not 18 bytes.
      while (LoRa.available()) {
        LoRa.read();
      }
    }
  }
}

// Function to decode Hamming(7,4) code and correct single-bit errors.
byte hammingDecode(byte encoded) {
  byte p1 = (encoded >> 6) & 0x1;
  byte p2 = (encoded >> 5) & 0x1;
  byte p3 = (encoded >> 4) & 0x1;
  byte d1 = (encoded >> 3) & 0x1;
  byte d2 = (encoded >> 2) & 0x1;
  byte d3 = (encoded >> 1) & 0x1;
  byte d4 = (encoded >> 0) & 0x1;

  byte s1 = p1 ^ d1 ^ d2 ^ d4;
  byte s2 = p2 ^ d1 ^ d3 ^ d4;
  byte s3 = p3 ^ d2 ^ d3 ^ d4;

  byte errorPos = (s3 << 2) | (s2 << 1) | s1;

  if (errorPos > 0 && errorPos <= 7) {
    encoded ^= (1 << (7 - errorPos));
  }

  return (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
}
