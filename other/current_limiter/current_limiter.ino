#include "current_sensor.h"

VoltageSensor senseV1(A6);
VoltageSensor senseV2(A5);
CurrentSensor currentSensor(&senseV1,&senseV2);

void setup() {
  //pinMode(A6,INPUT);
  Serial.begin(9600);
}

long long last_print = 0;

void printInfo() {
  last_print = millis();
  float v1 = currentSensor.v1();
  float v2 = currentSensor.v2();
  float current = currentSensor.getCurrent();
  //int shuntVal = currentSensor.val();
  //Serial.print("Shunt Val: ");
  //Serial.print(shuntVal);
  Serial.println(" Shunt Voltage: ");
  Serial.println(v1);
  Serial.println(v2);
  Serial.println(v1-v2);
  Serial.print(" Current: ");
  Serial.print(current);
  Serial.println(" A");
}

void loop() {
  long long now = millis();
  //if((now - last_print) > 500) printInfo();
  //currentSensor.updateValueFiltered(15,10);
  currentSensor.updateValue();
  printInfo();
  delay(500);
}
