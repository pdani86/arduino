
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
  int r_shunt; // mOhm

  VoltageSensor* vs1;
  VoltageSensor* vs2;

  float current;
  float _lastV1;
  float _lastV2;

public:
  
  inline CurrentSensor(VoltageSensor* _vs1,VoltageSensor* _vs2);

  inline float updateValue() {
    long sumV1 = 0;
    long sumV2 = 0;
    int nSamples = 1;
    for(int i=0;i<nSamples;i++) {
      if(vs1) {
        vs1->updateValue();
        vs1->updateValue();
        sumV1 += vs1->val();
      }
      if(vs2) {
        vs2->updateValue();
        vs2->updateValue();
        sumV2 += vs2->val();
      }
    }
    float constFactor = 5.0f/1023.0f/nSamples;
    float v1 = sumV1*constFactor;
    float v2 = sumV2*constFactor;
    _lastV1 = v1;
    _lastV2 = v2;
    float v_shunt = v1-v2;
    current = v_shunt/r_shunt*1000.0f;
    return current;
   }
  
  inline int rShunt() {return r_shunt;}
  inline void rShunt(int _r_shunt) {r_shunt = _r_shunt;}
  inline float v1() {return _lastV1;}
  inline float v2() {return _lastV2;}
  inline float vDiff() {return v1()-v2();}

  inline float getCurrent() {
    return current;
  }
};

CurrentSensor::CurrentSensor(VoltageSensor* _vs1,VoltageSensor* _vs2) {
  r_shunt = 100; // mOhm
  current = 0;
  vs1 = _vs1;
  vs2 = _vs2;
}

