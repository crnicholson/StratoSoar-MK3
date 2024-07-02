#ifndef GPS_H
#define GPS_H

void gpsSetup();
void displayInfo();
void gpsSleepTime(long);
void gpsSleep();
void gpsWakeup(bool = true);

#endif