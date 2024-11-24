/*
wifi.h, part of StratoSoar MK3, for an autonomous glider.
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

#include "websockets.h"

uint32_t getChipId() {
  return (uint32_t)ESP.getEfuseMac() >> 32; // Extract unique ID from efuse
}

void websocketsSetup() {
  WiFi.begin(YOUR_SSID, YOUR_PASSWORD);

#ifdef DEVMODE
  Serial.println("Connecting to WiFi...");
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEVMODE
    Serial.print(".");
#endif
  }
#ifdef DEVMODE
  Serial.println("Connected to WiFi, now connecting to server.");
#endif

  // Try to connect to Websockets server.
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connected) {
#ifdef DEVMODE
    Serial.println("Connected to server!");
#endif
  } else {
#ifdef DEVMODE
    Serial.println("Failed to connect to server. Freezing sketch.");
#endif
    while (1)
      longBlink(LED);
  }

  // Run callback when messages are received.
  client.onMessage([&](WebsocketsMessage message) {
#ifdef DEVMODE
    Serial.print("Got Message: ");
    Serial.println(message.data());
#endif
  });
}

void sendToServer() {
  JsonDocument doc;
  doc["lat"] = receivedData.lat;
  doc["lon"] = receivedData.lon;
  doc["altitude"] = receivedData.altitude;
  doc["tLat"] = receivedData.tLat;
  doc["tLon"] = receivedData.tLon;
  doc["temperature"] = receivedData.temperature;
  doc["pressure"] = receivedData.pressure;
  doc["humidity"] = receivedData.humidity;
  doc["volts"] = receivedData.volts;
  doc["yaw"] = receivedData.yaw;
  doc["pitch"] = receivedData.pitch;
  doc["roll"] = receivedData.roll;
  doc["hour"] = receivedData.hour;
  doc["year"] = receivedData.year;
  doc["month"] = receivedData.month;
  doc["day"] = receivedData.day;
  doc["minute"] = receivedData.minute;
  doc["second"] = receivedData.second;
  doc["abort"] = receivedData.abort;
  doc["txCount"] = receivedData.txCount;
  doc["rxCount"] = rxCount;
  doc["uLat"] = U_LAT; // Uploader location!
  doc["uLon"] = U_LON;
  doc["uAlt"] = U_ALT;
  doc["rssi"] = rssi;
  doc["snr"] = snr;
  doc["callsign"] = receivedData.callSign;
  doc["id"] = getChipId();

  String requestBody;
  serializeJson(doc, requestBody);

#ifdef DEVMODE
  Serial.print("Sending JSON: ");
  Serial.println(requestBody);
#endif

  client.send(requestBody);
}
