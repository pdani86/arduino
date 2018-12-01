#include <SPI.h>


class UltrasonicSensor
{
public:
  inline UltrasonicSensor() {
    lastTriggerUs = micros();
    lastDistCm = 0;
    triggerPin = 7;
    echoPin = 3;
  };

  inline void init() {
    _instance = this;
    pinMode(triggerPin,OUTPUT);
    pinMode(echoPin,INPUT_PULLUP);
    digitalWrite(triggerPin,LOW);

    attachInterrupt(digitalPinToInterrupt(echoPin), UltrasonicSensor::_onEchoChange, CHANGE);
  }

  static inline int pulseDurationUsToCm(unsigned long long us) {
    return us/58;
  }
  
  inline void trigger() {
     digitalWrite(triggerPin,HIGH);
    lastTriggerUs=micros();
    delayMicroseconds(10);
    digitalWrite(triggerPin,LOW);
  }
  void onEchoChange() {
    bool high = digitalRead(echoPin);
    if(high) lastEchoStartUs = micros();
    else {
      unsigned long long diffUs = micros() - lastEchoStartUs;
      lastDistCm = pulseDurationUsToCm(diffUs);
    }
  }
  
  static UltrasonicSensor* _instance;
  
  static void _onEchoChange();

  inline long distanceCm() {return lastDistCm;}
  
private:
  unsigned char triggerPin;
  unsigned char echoPin;
  unsigned long long lastTriggerUs;
  unsigned long long lastEchoStartUs;
  long lastDistCm;
};

void UltrasonicSensor::_onEchoChange() {
  //Serial.println("EchoChange");
  _instance->onEchoChange();
}

UltrasonicSensor* UltrasonicSensor::_instance = 0;

UltrasonicSensor ultrasonic;

class Thermistor
{
public:
  inline Thermistor(int pin):pin(pin) {
    R = 10000;
    rNominal = 5000.0f;
    bCoeff = 3950;
    tempNominal = 25.0f;
  }
  inline void init() {pinMode(pin,INPUT);}
  float getResistance();
  float getTemperature();

  float resistanceToTemperature(float rRead);
  static float resistanceToTemperature(float rRead,long rNominal,float bCoeff,float tempNominal);
private:
  int pin;
  float R; // added series resistance
  
  long rNominal;
  float bCoeff;
  float tempNominal;
};

float Thermistor::resistanceToTemperature(float rRead) {
  return resistanceToTemperature(rRead,rNominal,bCoeff,tempNominal);
}

float Thermistor::getResistance() {
  int x = analogRead(pin);
  //Serial.println(x);
  float r = R/(1023.0f/x-1);
  //Serial.print("R: ");
  //Serial.println(r);
  return r;
}

float Thermistor::getTemperature() {
  float r = getResistance();
  return resistanceToTemperature(r,rNominal,bCoeff,tempNominal);
}

float Thermistor::resistanceToTemperature(float rRead,long rNominal,float bCoeff,float tempNominal) {
  float steinhart;
  steinhart = rRead / rNominal;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= bCoeff;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (tempNominal + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  return steinhart;
}

Thermistor thermistor(A1);

void displayFloatAsFixPoint(float val,int n) {
  //ret = SPI.transfer();
}

#define DISPLAY_CS_PIN 8

void displayWriteReg(unsigned char addr,unsigned char val) {
  digitalWrite(DISPLAY_CS_PIN,LOW);
  SPI.transfer(addr);
  SPI.transfer(val);
  digitalWrite(DISPLAY_CS_PIN,HIGH);
  delay(1);
}

void displayTest(bool on) {
  displayWriteReg(0x0f,on?1:0);
}

void displaySetDecodeMode(bool set=true) {
  displayWriteReg(0x09,set?0xff:0x00);
}

void setDigit(unsigned char ix,unsigned char val) {
  displayWriteReg(1+ix,val);
}

void setIntensity(unsigned char val) {
  displayWriteReg(0x0a,val);
}

void setup() {
  Serial.begin(9600);
  thermistor.init();
  ultrasonic.init();

  pinMode(DISPLAY_CS_PIN,OUTPUT);
  digitalWrite(DISPLAY_CS_PIN,HIGH);

  SPI.begin();
  delay(10);
  setIntensity(0x0f); // max
  // scan all 8 characters
  displayWriteReg(0x0b,0x07);
  // turn on normal mode
  displayWriteReg(0x0c,0x01);
  // turn off display test
  displayTest(false);
  displaySetDecodeMode();
  for(int i=0;i<8;i++) displayWriteReg(1+i,0x0f); // clear display
}


unsigned char test = 0;

void displayTemperature(float& t) {
  int val = t*10;
  bool negative = false;
  //Serial.println(val);
  if(val<0) {
    //setDigit(7,0xA); // minus
    val *= -1;
    negative = true;
  } else {
    //setDigit(7,0xF); // blank
  }

  int m = 1;
  for(int i=0;i<4;i++) {
    char _val = (val/m)%10;
    if(i==1) _val |= 0x80;
    if(i==0 && negative) _val |= 0x80; // sign negative value with last decimal point (to save character space)
    if(i==3 && _val==0) _val = 0x0f;
    setDigit(i,_val);
    m*=10;
  }
}

void displayResistance(float& r) {
  int val = r;
  bool big = val>=10000;
  if(big) val/=10;
  int m = 1;
  for(int i=0;i<4;i++) {
    char _val = (val/m)%10;
    //if(i==3 && _val==0) _val = 0x0f;
    if(i==((big)?2:3)) _val |= 0x80;
    setDigit(i+4,_val);
    m*=10;
  }
}

void displayNaN() {
  for(int i=0;i<4;i++) setDigit(i+4,0x0A);
}

void displayCm(long val) {
  if(val>3000) {displayNaN(); return;}
  bool big = val>=10000;
  if(big) val/=10;
  int m = 1;
  for(int i=0;i<4;i++) {
    char _val = (val/m)%10;
    //if(i==3 && _val==0) _val = 0x0f;
    if(i==((big)?1:2)) _val |= 0x80;
    setDigit(i+4,_val);
    m*=10;
  }
}

void loop() {
  const char N = 5;
  float temperature;
  //temperature = thermistor.getTemperature();
  float resistance = thermistor.getResistance();
  
  for(int i=1;i<N;i++) {
    //temperature += thermistor.getTemperature();
    resistance += thermistor.getResistance();
  }
  //temperature /= N;
  resistance /= N;
  temperature = thermistor.resistanceToTemperature(resistance);

  /*Serial.print("R: ");
  Serial.println(resistance);
  Serial.print("T: ");
  Serial.println(temperature);*/
  
  displayTemperature(temperature);
  //displayResistance(resistance);

  long cm = (float)ultrasonic.distanceCm();
  displayCm(cm);

  //Serial.println(cm);

  ultrasonic.trigger();
 
  delay(80);
}

