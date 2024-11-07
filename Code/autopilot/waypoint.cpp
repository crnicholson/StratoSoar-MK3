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
  float wLon = coordinates[waypointCounter];
  waypointCounter++;
  if (waypointCounter == waypointIndexes) {
    return 0.0, 0.0;
  }

  return wLat, wLon;
}

// Return current waypoint count and total waypoints count.
int getWaypointCount() {
  return (waypointCounter + 1) / 2, waypoints;
}

// Only sets a new waypoint if conditions are right.
void updateWaypoint() {
  int x, y = getWaypointCount();
  if (!(x / y)) {
    if (distance <= CHANGE_WAYPOINT) {
      float prevTLat = targetLat, prevTLon = targetLon;
      targetLat, targetLon = returnNextWaypoint();
      if (targetLat == 0.0 || targetLon == 0.0) {
        targetLat = prevTLat;
        targetLon = prevTLon;
      }
#ifdef SMOOTH_TURNING
      getNextTurnWaypoint(); // Gets new targetLat and targetLon and puts it into the global variables.
#endif
    }
  } else {
    targetLat = TARGET_LAT;
    targetLon = TARGET_LON;
  }
}

// Commented out because it's old code.
// bool closeToPoint;
// bool foundCoords;

// // Only sets a new waypoint if conditions are right. NEED TO WORK ON THIS.
// void updateWaypointNew() {
//   if (distance <= CHANGE_WAYPOINT) {
//     closeToPoint = true;
//   } else {
//     closeToPoint = false;
//   }
//   if (closeToPoint && !foundCoords) {
//     float currentTargetLat, currentTargetLon = getCurrentWaypoint();
//     float nextTargetLat, nextTargetLon = returnNextWaypoint();
//   }
//   if (closeToPoint && foundCoords) {
//     float turnLat, turnLon = getNextTurnWaypoint();
//   }
// }

// When getting closer to the waypoint, the plane should start turning to face the next waypoint, smoothly.
void getNextTurnWaypoint() {
  float turnRate = 0.001; // Adjust this value for the desired turn sharpness.
  float stepDistance = 0.0001;

  float heading = yaw;

  // Calculate the heading towards the next waypoint.
  float desiredHeadingToNext = atan2(targetLon - lon, targetLat - lat);

  // Calculate the difference between the current heading and the desired heading to the next waypoint.
  float headingDifference = desiredHeadingToNext - heading;

  // Normalize the heading difference to the range -PI to PI.
  if (headingDifference > M_PI)
    headingDifference -= 2 * M_PI;
  if (headingDifference < -M_PI)
    headingDifference += 2 * M_PI;

  // Adjust the heading (heading) gradually towards the heading to the next waypoint.
  heading += headingDifference * turnRate;

  // Calculate the new target latitude and longitude based on the updated heading.
  targetLat = lat + stepDistance * cos(heading);
  targetLon = lon + stepDistance * sin(heading);
}

#define RADIUS diameter / 2.0
#define EARTH_RADIUS 6371000.0 // Earth radius in meters.
#define CIRCLE_POINTS 20

void getNextCircleWaypoint(float centerLat, float centerLon, int diameter) {
  // Calculate the angle from the center to the current position.
  double angle = atan2(lon - centerLon, lat - centerLat);

  // Calculate the next angle by adding a small increment (controls the smoothness).
  double angleIncrement = 2 * M_PI / CIRCLE_POINTS;
  angle += angleIncrement;

  targetLat = centerLat + (RADIUS / EARTH_RADIUS) * cos(angle) * (180.0 / M_PI);
  targetLon = centerLon + (RADIUS / EARTH_RADIUS) * sin(angle) * (180.0 / M_PI) / cos(centerLat * M_PI / 180.0); // Return the lat, lon.
}