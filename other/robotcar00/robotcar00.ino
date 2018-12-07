
class MyMotor
{
  byte enPin;
  byte in1Pin;
  byte in2Pin;
  bool _invertDir;
  
public:
  inline MyMotor(byte _enPin,byte _in1Pin,byte _in2Pin):enPin(_enPin),in1Pin(_in1Pin),in2Pin(_in2Pin) {_invertDir=false;}
  inline init() {pinMode(enPin,OUTPUT); pinMode(in1Pin,OUTPUT); pinMode(in2Pin,OUTPUT);}
  void pwm(byte val,bool dir = true);
  void brake(byte val,bool dir = false);
  void enable(bool en);
  inline void invertDir(bool inv) {_invertDir=inv;};
};

void MyMotor::enable(bool en) {
  digitalWrite(enPin,en);
}

void MyMotor::pwm(byte val,bool dir) {
  if(_invertDir) dir = !dir;
  analogWrite(enPin,val);
  digitalWrite(in1Pin,dir);
  digitalWrite(in2Pin, (0==val)?dir:(!dir) );
}

void MyMotor::brake(byte val,bool dir) {
  analogWrite(enPin,val);
  digitalWrite(in1Pin,dir);
  digitalWrite(in2Pin,dir);
}

#define REAR_IR_PIN 4

#define EN_RIGHT_SIDE_PIN 5
#define PWM_RIGHT_SIDE_1_PIN A0
#define PWM_RIGHT_SIDE_2_PIN A1

#define EN_LEFT_SIDE_PIN 6
#define PWM_LEFT_SIDE_1_PIN 7
#define PWM_LEFT_SIDE_2_PIN 8


MyMotor motorRight(EN_RIGHT_SIDE_PIN,PWM_RIGHT_SIDE_1_PIN,PWM_RIGHT_SIDE_2_PIN);
MyMotor motorLeft(EN_LEFT_SIDE_PIN,PWM_LEFT_SIDE_1_PIN,PWM_LEFT_SIDE_2_PIN);

void setup() {
  // put your setup code here, to run once:
  pinMode(REAR_IR_PIN,INPUT);
  motorLeft.init();
  motorRight.init();
  motorRight.invertDir(true);
  /*pinMode(EN_RIGHT_SIDE_PIN,OUTPUT);
  pinMode(PWM_RIGHT_SIDE_1_PIN,OUTPUT);
  pinMode(PWM_RIGHT_SIDE_2_PIN,OUTPUT);
  pinMode(EN_LEFT_SIDE_PIN,OUTPUT);
  pinMode(PWM_LEFT_SIDE_1_PIN,OUTPUT);
  pinMode(PWM_LEFT_SIDE_2_PIN,OUTPUT);*/
}





void forward(byte val) {
  motorRight.pwm(val);
  motorLeft.pwm(val);
}

void stopCar() {
  motorRight.brake(255);
  motorLeft.brake(255);
}

void loop() {
  bool obstacleRear = !digitalRead(REAR_IR_PIN);
  if(obstacleRear) {
    forward(255);
    delay(50);
  } else {
    stopCar();
    delay(50);
  }
}
