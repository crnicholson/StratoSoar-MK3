// Derived by SJR from Example1_Basics.ino by Owen Lyke @ SparkFun Electronics, 2019, and further modified by Charlie Nicholson, 2024.

#include <ICM_20948.h> // You need the Sparkfun ICM-20948 library.

// Settings.
#define AD0_VAL 0 // Change I2C address. 0 = 0x68, 1 = 0x69.
#define BAUD_RATE 115200

// Gyro offset values for calibration.
long gyro[3] = {0};
int offsetCount = 500;  // Average this many values for gyro.
int accMagCount = 300; // Collect this many values for acc/mag calibration.

bool magInit, imuInit;

float gOff;
int i;

ICM_20948_I2C imu; // Creat the ICM_20948_I2C object.

void setup() {
  SerialUSB.begin(BAUD_RATE);
  while (!SerialUSB)
    ;

  Wire.begin();
  Wire.setClock(400000);

  // imu.enableDebugging(); // Uncomment this line to enable helpful debug messages on serial.

  while (!imuInit || !magInit) {
    imu.begin(Wire, AD0_VAL);

    SerialUSB.print("Initialization of the IMU returned: ");
    SerialUSB.println(imu.statusString());
    if (imu.status != ICM_20948_Stat_Ok) {
      SerialUSB.println("Trying again...");
      delay(500);
    } else {
      imuInit = true;
    }

    imu.startupMagnetometer();
    SerialUSB.print(F("Initialization of the magnetometer returned: "));
    SerialUSB.println(imu.statusString());
    if (imu.status != ICM_20948_Stat_Ok) {
      SerialUSB.println("Trying again...");
      delay(500);
    } else {
      magInit = true;
    }
  }

  SerialUSB.println("ICM-20948 connected. Sketch starting shortly.");

  SerialUSB.println("Hold sensor still for gyro offset calibration ...");
  delay(5000);

  for (i = 0; i < offsetCount; i++) {
    if (imu.dataReady()) {
      imu.getAGMT(); // The values are only updated when you call 'getAGMT'.
      gyro[0] += imu.agmt.gyr.axes.x;
      gyro[1] += imu.agmt.gyr.axes.y;
      gyro[2] += imu.agmt.gyr.axes.z;
    }
  }

  SerialUSB.print("Gyro offsets (x, y, z): ");

  for (i = 0; i < 3; i++) {
    gOff = (float)gyro[i] / offsetCount;
    SerialUSB.print(gOff, 1);
    SerialUSB.print(", ");
  }

  SerialUSB.print("\nTurn sensor slowly and steadly in 3D space until "); // Moving on to acc/mag calibration.
  SerialUSB.print(accMagCount);
  SerialUSB.println(" values are collected.");
  delay(1000);
  SerialUSB.println("Starting now...");

  // Get values for calibration of acc/mag.
  for (i = 0; i < accMagCount; i++) {
    if (imu.dataReady()) {
      imu.getAGMT(); // The values are only updated when you call 'getAGMT'.

      SerialUSB.print(imu.agmt.acc.axes.x);
      SerialUSB.print(",");
      SerialUSB.print(imu.agmt.acc.axes.y);
      SerialUSB.print(",");
      SerialUSB.print(imu.agmt.acc.axes.z);
      SerialUSB.print(",");

      SerialUSB.print(imu.agmt.mag.axes.x);
      SerialUSB.print(",");
      SerialUSB.print(imu.agmt.mag.axes.y);
      SerialUSB.print(",");
      SerialUSB.println(imu.agmt.mag.axes.z);

      delay(200);
    } else {
      delay(100); // Wait for data ready.
    }
  }
  SerialUSB.print("Done collecting");
}

void loop() {}