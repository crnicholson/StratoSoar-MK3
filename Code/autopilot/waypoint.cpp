#include "waypoint.h"

int waypoints;
int waypointCounter;
int waypointIndexes;

double coordinates[] = {-71.36429369651623, 42.31591837810452, -71.3633453852528, 42.31648142887113, -71.3639570792646, 42.3173739762732, -71.36498753851305, 42.31680590512546, -71.36429369651623, 42.31591837810452};

void waypointsSetup() {
  waypoints = sizeof(coordinates) / 2;
  waypointIndexes = sizeof(coordinates) - 1;
#ifdef DEVMODE
  SerialUSB.print("There are ");
  SerialUSB.print(waypoints);
  SerialUSB.println(" waypoints");
  SerialUSB.print("Current target lat/lon: ");
  SerialUSB.print(coordinates[0]);
  SerialUSB.print(", ");
  SerialUSB.println(coordinates[1]);
#endif
}

float returnNextWaypoint() {
  float wLat = coordinates[waypointCounter];
  waypointCounter++;
  if (waypointCounter == waypointIndexes) {
    return 0.0, 0.0;
  }
  float WLon = coordinates[waypointCounter];
  waypointCounter++;
  if (waypointCounter == waypointIndexes) {
    return 0.0, 0.0;
  }

  return wLat, WLon;
}

// Return current waypoint and total waypoints.
int getCurrentWaypoint() {
  return (waypointCounter + 1) / 2, waypoints;
}

// Only sets a new waypoint if conditions are right.
void updateWaypoint() {
  int x, y = getCurrentWaypoint();
  if (!(x / y)) {
    if (distance <= CHANGE_WAYPOINT) {
      float prevTLat = targetLat, prevTLon = targetLon;
      targetLat, targetLon = returnNextWaypoint();
      if (targetLat == 0.0 || targetLon == 0.0) {
        targetLat = prevTLat;
        targetLon = prevTLon;
      }
    }
  } else {
    targetLat = TARGET_LAT;
    targetLon = TARGET_LON;
  }
}

// When getting closer to the waypoint, the plane should start turning to face the next waypoint, smoothly.
float getNextTurnWaypoint() {
  float turnRate = 0.1;
  float stepDistance = 0.0001;

  float heading = yaw;

  // Calculate the desired heading towards the target.
  float desiredHeading = atan2(targetLon - lon, targetLat - lat);

  if (heading < desiredHeading) {
    heading += turnRate;
    if (heading > desiredHeading)
      heading = desiredHeading;
  } else if (heading > desiredHeading) {
    heading -= turnRate;
    if (heading < desiredHeading)
      heading = desiredHeading;
  }

  // Calculate the next waypoint using the current heading.
  float nextLat = currentLat + stepDistance * cos(heading);
  float nextLon = currentLon + stepDistance * sin(heading);
  return nextLat, nextLon;
}

#define RADIUS diameter / 2.0
#define EARTH_RADIUS 6371000.0 // Earth radius in meters.
#define CIRCLE_POINTS 20

float getNextCircleWaypoint(float centerLat, float centerLon, int diameter) {
  // Calculate the angle from the center to the current position.
  double angle = atan2(lon - centerLon, lat - centerLat);

  // Calculate the next angle by adding a small increment (controls the smoothness).
  double angleIncrement = 2 * M_PI / CIRCLE_POINTS;
  angle += angleIncrement;

  float nextLat = centerLat + (RADIUS / EARTH_RADIUS) * cos(angle) * (180.0 / M_PI);
  float nextLon = centerLon + (RADIUS / EARTH_RADIUS) * sin(angle) * (180.0 / M_PI) / cos(centerLat * M_PI / 180.0);
  return nextLat, nextLon;
}