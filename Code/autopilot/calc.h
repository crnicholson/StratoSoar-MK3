#pragma once

#include "settings.h"
#include <Arduino.h>

// cLat = currentLat
// cLon = currentLon
// tLat = targetLat
// tLon = targetLon
// heading = yaw
double toRadians(double deg);
double toDegrees(double rad);
double azimuth(double cLat, double cLon, double tLat, double tLon);
double turningAngle(double cLat, double cLon, double heading, double tLat, double tLon);
double calculateDistance(double cLat, double cLon, double tLat, double tLon);
