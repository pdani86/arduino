
class VoltageSensor {
  const int ANALOG_MAX = 1023;
  int _pin;
  int v_ref; // mV
  unsigned int value;

public:
  inline VoltageSensor(int pin = A6);
  inline int vRef() {return v_ref;}
  inline void vRef(int _v_ref) {v_ref = _v_ref;}

  int updateValue();
  int updateValueFiltered(int nSamples,int delayUs = 0);
  inline unsigned int val() {return value;}

  inline float getVoltage();
};

int VoltageSensor::updateValueFiltered(int nSamples,int delayUs) {
  long long sum = 0;
  sum += updateValue();
  for(int i=1;i<nSamples;i++) {
    if(delayUs>0) delayMicroseconds(delayUs);
    sum += updateValue();
  }
  value = sum/nSamples;
  return value;
}

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
   return (value = analogRead(_pin));
}

class CurrentSensor {
  int r_shunt; // mOhm (resistance to ground)

  VoltageSensor* vs1;
  VoltageSensor* vs2;

  float current;

public:
  
  inline CurrentSensor(VoltageSensor* _vs1,VoltageSensor* _vs2);

  inline float updateValue() {
    if(vs1) vs1->updateValue();
    delayMicroseconds(50);
    if(vs2) vs2->updateValue();
    float v1 = (vs1)?(vs1->getVoltage()):0.0f;
    float v2 = (vs2)?(vs2->getVoltage()):0.0f;
    float v_shunt = v1-v2;
    float current = v_shunt/r_shunt*1000.0f;
   }
  
  inline int rShunt() {return r_shunt;}
  inline void rShunt(int _r_shunt) {r_shunt = _r_shunt;}
  inline float v1() {if(0==vs1) return 0.0f; return vs1->getVoltage();}
  inline float v2() {if(0==vs2) return 0.0f; return vs2->getVoltage();}
  inline float vDiff() {return v1()-v2();}

  inline float getCurrent() {
    float v_shunt = vDiff();
    current = v_shunt/r_shunt*1000.0f;
    return current;
  }
};

CurrentSensor::CurrentSensor(VoltageSensor* _vs1,VoltageSensor* _vs2) {
  r_shunt = 133; // mOhm
  current = 0;
  vs1 = _vs1;
  vs2 = _vs2;
}

