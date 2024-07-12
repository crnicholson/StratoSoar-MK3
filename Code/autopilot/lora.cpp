#include "lora.h"

void loraSetup() {
  if (!LoRa.begin(FREQUENCY)) {
#ifdef DEVMODE
    SerialUSB.println("Starting LoRa failed!");
#endif
    while (1) {
      longBlink(ERR_LED);
    }
  }

  LoRa.setSyncWord(SYNC_WORD);               // Defined in settings.h.
  LoRa.setSpreadingFactor(SPREADING_FACTOR); // Defined in settings.h.
  LoRa.setSignalBandwidth(BANDWIDTH);        // Defined in settings.h.
  LoRa.crc();                                // Checksum for packet error detection.
}

void sendData() {
  while (LoRa.beginPacket() == 0) {
    delay(10);
  }

  LoRa.beginPacket();
  LoRa.write((byte *)&data, sizeof(data));
  LoRa.endPacket(true);
}