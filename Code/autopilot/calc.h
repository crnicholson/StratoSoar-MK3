#pragma once

#include "settings.h"
#include <Arduino.h>

double toRadians(double deg);
double toDegrees(double rad);
double azimuth(double cLat, double cLon, double tLat, double tLon);
double turningAngle(double cLat, double cLon, double head, double tLat, double tLon);
double calculateDistance(double cLat, double cLon, double tLat, double tLon);
