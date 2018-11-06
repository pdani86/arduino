#include "current_sensor.h"

CurrentSensor currentSensor(A6);

void setup() {
  //pinMode(A6,INPUT);
  Serial.begin(9600);
}

void loop() {
  currentSensor.updateValue();

  float v = currentSensor.getVoltage();
  float current = currentSensor.getCurrent();
  int shuntVal = currentSensor.val();
  Serial.print("Shunt Val: ");
  Serial.print(shuntVal);
  Serial.print(" Shunt Voltage: ");
  Serial.print(v);
  Serial.print(" Current: ");
  Serial.print(current);
  Serial.println(" A");
  delay(500);
}
