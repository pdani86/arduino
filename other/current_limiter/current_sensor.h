
class VoltageSensor {
  const int ANALOG_MAX = 1023;
  int _pin;
  int v_ref; // mV
  int value;

public:
  inline VoltageSensor(int pin = A6);
  inline int vRef() {return v_ref;}
  inline void vRef(int _v_ref) {v_ref = _v_ref;}

  int updateValue();
  inline int val() {return value;}

  inline float getVoltage();
};

float VoltageSensor::getVoltage() {
  float voltage = v_ref*((long long)value)/(float)ANALOG_MAX;
  return voltage/1000.0f;
}

VoltageSensor::VoltageSensor(int pin):ANALOG_MAX(1023) {
  _pin = pin;
  value = 0;
  v_ref = 5000; // mV
  pinMode(_pin,INPUT);
}

int VoltageSensor::updateValue() {
   return (value = analogRead(A6));
}

class CurrentSensor:public VoltageSensor {
  int r_shunt; // mOhm (resistance to ground)

public:
  
  inline CurrentSensor(int pin = A6);
  
  inline int rShunt() {return r_shunt;}
  inline void rShunt(int _r_shunt) {r_shunt = _r_shunt;}

  inline float getCurrent() {
    float v_shunt = getVoltage();
    float current = v_shunt/r_shunt*1000.0f;
    return current;
  }
};

CurrentSensor::CurrentSensor(int pin) {
  r_shunt = 174; // mOhm
}

