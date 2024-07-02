#include "servo.h"

#define KP_LEFT 1
#define KI_LEFT 0
#define KD_LEFT 0

#define KP_RIGHT 1
#define KI_RIGHT 0
#define KD_RIGHT 0

#define SETPOINT_PITCH 0

int errorPitch, errorYaw, prevErrorPitch, prevErrorYaw, integralPitch, integralYaw;

int pidElevons(int pitch, int yaw) {
  errorPitch = SETPOINT_PITCH - pitch;
  errorYaw = turningAngle;

  int outputPitch = KP_LEFT * errorPitch + KI_LEFT * integralPitch + KD_LEFT * (errorPitch - prevErrorPitch);
  int outputYaw = KP_RIGHT * errorYaw + KI_RIGHT * integralYaw + KD_RIGHT * (errorYaw - prevErrorYaw);

  prevErrorPitch = errorPitch;
  integralPitch += errorPitch;

  prevErrorYaw = errorYaw;
  integralYaw += errorYaw;

  servoPositionLeft = 90 - outputPitch + outputYaw;
  servoPositionRight = 90 - outputPitch - outputYaw;

  // Map servo positions if needed.
  // servoPositionLeft = map(servoPositionLeft, 0, 180, 750, 2250);
  // servoPositionRight = map(servoPositionRight, 0, 180, 750, 2250);

  return servoPositionLeft, servoPositionRight;
}