#include "calc.h"

double toRadians(double deg) {
  return deg * PI / 180.0;
}

double toDegrees(double rad) {
  return rad * 180.0 / PI;
}

double azimuth(double cLat, double cLon, double tLat, double tLon) {
  double dLon = toRadians(tLon - cLon);
  double dPhi = log(tan(toRadians(tLat) / 2 + PI / 4) / tan(toRadians(cLat) / 2 + PI / 4));
  if (fabs(dLon) > PI) {
    dLon = dLon > 0 ? -(2 * PI - dLon) : (2 * PI + dLon);
  }
  return fmod((atan2(dLon, dPhi) + 2 * PI), (2 * PI));
}

double turningAngle(double cLat, double cLon, double head, double tLat, double tLon) {
  double a = azimuth(cLat, cLon, tLat, tLon);
  double h = toRadians(head);
  double angle = a - h;
  if (angle < -PI)
    angle += 2 * PI;
  if (angle > PI)
    angle -= 2 * PI;
  return toDegrees(angle);
}

// Haversine formula to calculate distance between two coordinates.
double calculateDistance(double cLat, double cLon, double tLat, double tLon) {
  double dLat = toRadians(tLat - cLat);
  double dLon = toRadians(tLon - cLon);
  double a = sin(dLat / 2) * sin(dLat / 2) + cos(toRadians(tLat)) * cos(toRadians(tLat)) * sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double distance = 6371000 * c;
  return distance;
}