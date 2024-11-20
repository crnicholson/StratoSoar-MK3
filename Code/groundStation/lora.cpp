/*
lora.cpp part of StratoSoar MK3, for an autonomous glider.
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

#include "lora.h"

void loraSetup() {
  LoRa.setPins(LORA_SS_PIN, RESET_PIN, DIO0_PIN); // Has to be before LoRa.begin().

  if (!LoRa.begin(FREQUENCY)) {
#ifdef DEVMODE
    Serial.println("Starting LoRa failed!");
#endif
    while (1)
      longBlink(LED);
  }

#ifdef DEVMODE
  Serial.println("LoRa succesfully initialized!");
#endif

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
}

void normalReceive() {
  while (LoRa.available() < sizeof(receiveStruct)) // Wait intil the packet is fully received.
    ;

  packetSize = LoRa.parsePacket();
  if (packetSize == sizeof(receiveStruct)) {
    LoRa.readBytes((byte *)&receiveStruct, sizeof(receiveStruct));

    dataValid = true;

    if (receiveStruct.alignmet != 100) {
      dataValid = false;
      while (LoRa.available())
        LoRa.read();
    }

    rxCount++;
  } else {
    while (LoRa.available())
      LoRa.read(); // Discard the packet if it's not the right size.
  }
}

void normalSend() {
  // Wait until the LoRa is ready to send a new packet.
  while (LoRa.beginPacket() == 0)
    ;

  LoRa.beginPacket();
  LoRa.write((byte *)&sendStruct, sizeof(sendStruct));
  LoRa.endPacket(true); // Use async send.
}

void hammingReceive() {
  while (LoRa.available() < sizeof(receiveStruct) * 2) // Wait intil the packet is fully received.
    ;

  packetSize = LoRa.parsePacket(); // Parse packet.
  if (sizeof(receiveStruct) * 2 == packetSize) {
    shortBlink(LED);

    byte encodedData[2 * sizeof(receiveStruct)];
    LoRa.readBytes(encodedData, sizeof(encodedData));

    byte *decodedData = (byte *)&receiveStruct;

    for (size_t i = 0; i < sizeof(receiveStruct); ++i) {
      // Decode each byte using Hamming(7,4) code.
      byte highNibble = hammingDecode(encodedData[2 * i]);
      byte lowNibble = hammingDecode(encodedData[2 * i + 1]);

      decodedData[i] = (highNibble << 4) | lowNibble;
    }

    dataValid = true;

    if (receiveStruct.alignmet != 100) {
      dataValid = false;
      while (LoRa.available())
        LoRa.read();
    }

    rxCount++;
  } else {
    while (LoRa.available())
      LoRa.read(); // Discard the packet if it's not the right size.
  }
}

void hammingSend() {
  byte *byteArray = (byte *)&sendStruct; // Convert the packet to a byte array.

  byte encodedData[2 * sizeof(sendStruct)]; // Hamming(7,4) doubles the size!

  for (size_t i = 0; i < sizeof(sendStruct); ++i) {
    byte highNibble = hammingEncode((byteArray[i] >> 4) & 0xF);
    byte lowNibble = hammingEncode(byteArray[i] & 0xF);

    encodedData[2 * i] = highNibble;
    encodedData[2 * i + 1] = lowNibble;
  }

  // Wait until the LoRa is ready to send a new packet.
  while (LoRa.beginPacket() == 0)
    ;

  LoRa.beginPacket();
  LoRa.write(encodedData, sizeof(encodedData));
  LoRa.endPacket(true); // Use async send.
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

  // Calculate syndrome bits.
  byte s1 = p1 ^ d1 ^ d2 ^ d4;
  byte s2 = p2 ^ d1 ^ d3 ^ d4;
  byte s3 = p3 ^ d2 ^ d3 ^ d4;

  // Determine error position.
  byte errorPos = (s3 << 2) | (s2 << 1) | s1;

  // Correct the error if needed.
  if (errorPos > 0 && errorPos <= 7) {
    encoded ^= (1 << (7 - errorPos));
  }

  return (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
}

byte hammingEncode(byte data) {
  byte p1 = (data >> 2) & 0x1 ^ (data >> 1) & 0x1 ^ (data >> 0) & 0x1;
  byte p2 = (data >> 3) & 0x1 ^ (data >> 1) & 0x1 ^ (data >> 0) & 0x1;
  byte p3 = (data >> 3) & 0x1 ^ (data >> 2) & 0x1 ^ (data >> 0) & 0x1;

  byte hamming = (p1 << 6) | (p2 << 5) | (p3 << 4) | (data & 0xF);
  return hamming;
}