// Enables.
#define DEVMODE // Toggle serial monitor.
#define USE_GPS
#define GPS_LOW_POWER

// Other.
#define UPDATE_RATE 100  // The time in milliseconds between glider respositioning.
#define BAUD_RATE 115200 // Baud rate of the serial monitor.

// Positioning.
#define GPS_BAUD_RATE 9600       // Baud rate of the GPS module.
#define GPS_UPDATE_RATE 5000     // The time in milliseconds stating the frequency of new GPS data.
#define GPS_LOW_POWER_RATE 15000 // The time in milliseconds stating the frequency of new GPS data in low power mode.
#define TARGET_LAT 41.40338      // Target latitude.
#define TARGET_LON -71.9533      // Target longitude.
#define TESTING_LAT 41.30338     // Testing latitude.
#define TESTING_LON -71.8533     // Testing longitude.

// Pins.
#define LED 13
#define ERR_LED 12
#define GPS_SLEEP_PIN 11
#define LEFT_SERVO 10
#define RIGHT_SERVO 9
#define LEFT_FET 8
#define RIGHT_FET 7
#define VOLTMETER_PIN A0

#define PI 3.1415926
