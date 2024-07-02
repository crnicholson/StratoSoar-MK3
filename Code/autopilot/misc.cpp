#include "misc.h"

float readVoltage() {
  int rawVolt = analogRead(VOLTMETER_PIN);
  rawVolt = rawVolt * 2;
  return rawVolt * (3.3 / 1023.0);
}