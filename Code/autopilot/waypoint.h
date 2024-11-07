#pragma once

#include "settings.h"
#include "vars.h"
#include <Arduino.h>

void waypointsSetup();
float returnNextWaypoint();
int getWaypointCount();
void updateWaypoint();
void getNextTurnWaypoint();
void getNextCircleWaypoint(float centerLat, float centerLon, int diameter);