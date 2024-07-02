#include "servo.h"

int pidMagicLeft() {
  errorLeft = SETPOINT_Left - pitch; // Calculate the error.

  int outputLeft = KP_Left * errorLeft + KI_Left * integralLeft + KD_Left * (errorLeft - prevErrorLeft); // Calculate the output.

  int servoPositionLeft = 90 - outputLeft; // Adjust servo position based on the output.

  // Update previous error and integral.
  prevErrorLeft = errorLeft;
  integralLeft += errorLeft;

  // int servoPositionLeftNew = map(servoPositionLeft, 0, 180, 750, 2250); // For servos that take microseconds not degrees.

  return servoPositionLeft;
}

int pidMagicRight() {
  setpointRight = data.yaw + data.turnAngle;
  inputRight = data.yaw;

  errorRight = setpointRight - data.yaw; // Calculate the error.

  int outputRight = KP_Right * errorRight + KI_Right * integralRight + KD_Right * (errorRight - prevErrorRight); // Calculate the output.

  int servoPositionRight = 180 + outputRight; // Adjust servo position based on the output.

  // Update previous error and integral.
  prevErrorRight = errorRight;
  integralRight += errorRight;

  // int servoPositionRight = map(servoPositionRight, 0, 180, 1250, 1750); // For servos that take microseconds not degrees.

  return servoPositionRight / 2;
}

// #include "servo.h"

// // Define constants for PID
// const int KP_Left = 1; // Proportional gain for left elevon
// const int KI_Left = 0; // Integral gain for left elevon
// const int KD_Left = 0; // Derivative gain for left elevon

// const int KP_Right = 1; // Proportional gain for right elevon
// const int KI_Right = 0; // Integral gain for right elevon
// const int KD_Right = 0; // Derivative gain for right elevon

// // Define setpoints for pitch and roll
// const int SETPOINT_Pitch = 0;
// const int SETPOINT_Roll = 0;

// // Variables for PID control
// int errorPitch, errorRoll;
// int prevErrorPitch = 0, prevErrorRoll = 0;
// int integralPitch = 0, integralRoll = 0;

// int pidMagicElevons(int pitch, int roll) {
//   // Calculate errors
//   errorPitch = SETPOINT_Pitch - pitch;
//   errorRoll = SETPOINT_Roll - roll;

//   // Calculate outputs
//   int outputPitch = KP_Left * errorPitch + KI_Left * integralPitch + KD_Left * (errorPitch - prevErrorPitch);
//   int outputRoll = KP_Right * errorRoll + KI_Right * integralRoll + KD_Right * (errorRoll - prevErrorRoll);

//   // Update previous errors and integrals
//   prevErrorPitch = errorPitch;
//   integralPitch += errorPitch;

//   prevErrorRoll = errorRoll;
//   integralRoll += errorRoll;

//   // Calculate servo positions for left and right elevons
//   int servoPositionLeft = 90 - outputPitch + outputRoll;
//   int servoPositionRight = 90 - outputPitch - outputRoll;

//   // Map servo positions if needed
//   // int servoPositionLeftNew = map(servoPositionLeft, 0, 180, 750, 2250);
//   // int servoPositionRightNew = map(servoPositionRight, 0, 180, 750, 2250);

//   // Return servo positions (if using microseconds, return mapped values)
//   return {servoPositionLeft, servoPositionRight};
// }
