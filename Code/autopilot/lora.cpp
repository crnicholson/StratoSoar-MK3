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
  while (LoRa.beginPacket() == 0) {
    delay(10);
  }

  LoRa.beginPacket();
  LoRa.write((byte *)&newPacket, sizeof(newPacket));
  LoRa.endPacket(true); // Use async send.
}