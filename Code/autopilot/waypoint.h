#pragma once

#include "settings.h"
#include "vars.h"
#include <Arduino.h>

void waypointsSetup();
float returnNextWaypoint();
int getCurrentWaypoinx();
void updateWaypoint();