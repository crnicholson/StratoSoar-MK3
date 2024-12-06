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
    JsonDocument doc;
    deserializeJson(doc, message.data());

    toGliderStruct.tLat = doc["tLat"];
    toGliderStruct.tLon = doc["tLon"];
    toGliderStruct.abort = doc["abort"];
    strcpy(toGliderStruct.callSign, doc["callSign"]);

#ifdef DEVMODE
    Serial.print("Raw data from server: ");
    Serial.println(message.data());
#endif

    newPacketForGlider = true;
  });
}

void sendToServer() {
  JsonDocument doc;
  doc["lat"] = fromGliderStruct.lat;
  doc["lon"] = fromGliderStruct.lon;
  doc["altitude"] = fromGliderStruct.altitude;
  doc["tLat"] = fromGliderStruct.tLat;
  doc["tLon"] = fromGliderStruct.tLon;
  doc["temperature"] = fromGliderStruct.temperature;
  doc["pressure"] = fromGliderStruct.pressure;
  doc["humidity"] = fromGliderStruct.humidity;
  doc["volts"] = fromGliderStruct.volts;
  doc["yaw"] = fromGliderStruct.yaw;
  doc["pitch"] = fromGliderStruct.pitch;
  doc["roll"] = fromGliderStruct.roll;
  doc["hour"] = fromGliderStruct.hour;
  doc["year"] = fromGliderStruct.year;
  doc["month"] = fromGliderStruct.month;
  doc["day"] = fromGliderStruct.day;
  doc["minute"] = fromGliderStruct.minute;
  doc["second"] = fromGliderStruct.second;
  doc["abort"] = fromGliderStruct.abort;
  doc["txCount"] = fromGliderStruct.txCount;
  doc["rxCount"] = rxCount;
  doc["uLat"] = U_LAT; // Uploader location!
  doc["uLon"] = U_LON;
  doc["uAlt"] = U_ALT;
  doc["rssi"] = rssi;
  doc["snr"] = snr;
  doc["callsign"] = fromGliderStruct.callSign;
  doc["user1"] = fromGliderStruct.user1;
  doc["user2"] = fromGliderStruct.user2;
  doc["user3"] = fromGliderStruct.user3;
  doc["user4"] = fromGliderStruct.user4;
  doc["user5"] = fromGliderStruct.user5;
  doc["password"] = fromGliderStruct.password;
  doc["id"] = getChipId();

  String requestBody;
  serializeJson(doc, requestBody);

#ifdef DEVMODE
  Serial.print("Sending JSON: ");
  Serial.println(requestBody);
#endif

  client.send(requestBody);
}
