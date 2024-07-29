#include "waypoint.h"

int waypoints;
int waypointCounter;

float waypointSetupint() {
  waypoints = sizeof(coordinates) / 2;
  SerialUSB.print("There are ");
  SerialUSB.print(waypoints);
  SerialUSB.println(" waypoints");
  SerialUSB.print("Current target lat/lon: ");
  SerialUSB.print(coordinates[0]);
  SerialUSB.print(", ");
  SerialUSB.println(coordinates[1]);

  return coordinates[0], coordinates[1];

  waypointCounter = 1;
}

float returnNextWaypoint() {
  float lat = coordinates[waypointCounter];
  waypointCounter++;
  float lon = coordinates[waypointCounter];
  waypointCounter++;

  if 
}