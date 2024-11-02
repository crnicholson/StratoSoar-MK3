#pragma once

#include "settings.h"
#include "vars.h"
#include <Arduino.h>

void waypointsSetup();
float returnNextWaypoint();
int getWaypointCount();
void updateWaypoint();
float getNextTurnWaypoint();
float getNextCircleWaypoint(float centerLat, float centerLon, int diameter);