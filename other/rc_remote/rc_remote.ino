
#include <SPI.h>
#include <RF24.h>

#define RF24_CE_PIN 7
#define RF24_CS_PIN 8

#define RADIO_CHANNEL 29
#define RC_ADDRESS 0x0086102400

RF24 radio(RF24_CE_PIN,RF24_CS_PIN);

#define X_AXIS_PIN A3
#define Y_AXIS_PIN A4
#define BTN_PIN 4


struct RADIO_CONTROL_PACKET
{
  char steer;
  byte throttle;
  byte brake;
  byte clutch;
  char gear;
  byte lamps;
  //byte 
};

class Joystick
{
  int xAxis;
  int yAxis;
  bool btn;
public:
  inline Joystick(byte xPin,byte yPin,byte btnPin):
    _xPin(xPin),_yPin(yPin),_btnPin(btnPin) {xAxis = yAxis = 0; btn = false;}
  inline void begin() {
    pinMode(_xPin,INPUT);
    pinMode(_yPin,INPUT);
    pinMode(_btnPin,INPUT);
  };
  void update();
  void fillControlPacket(RADIO_CONTROL_PACKET* packet);
private:
  byte _xPin;
  byte _yPin;
  byte _btnPin;
};

void Joystick::update() {
  analogRead(_xPin);
  xAxis = analogRead(_xPin);
  analogRead(_yPin);
  yAxis = analogRead(_yPin);
  btn = digitalRead(_btnPin);
}

void Joystick::fillControlPacket(RADIO_CONTROL_PACKET* packet) {
  packet->steer = map(xAxis,0,1023,-128,127);
  packet->throttle = map(abs(yAxis-512),0,512,0,255);
  packet->gear = (yAxis<512)?(-1):(1);
  packet->lamps = (btn)?(0xff):(0x00);
}

Joystick joy(X_AXIS_PIN,Y_AXIS_PIN,BTN_PIN);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Serial ready");
  radio.begin();
  //radio.setPALevel(RF24_PA_LOW);
  radio.setPALevel(RF24_PA_HIGH);
  //radio.setDataRate(RF24_250KBPS);
  //radio.setAutoAck(false);
  radio.setRetries(2,15);
  radio.setChannel(29);

  radio.openWritingPipe(RC_ADDRESS);
  radio.stopListening();

  joy.begin();
}

RADIO_CONTROL_PACKET control_packet;

void loop() {
  joy.update();
  joy.fillControlPacket(&control_packet);
  radio.write(&control_packet,sizeof(control_packet));
  Serial.println(control_packet.throttle);
  Serial.println((byte)control_packet.steer);
  Serial.println(control_packet.lamps);
  Serial.println();
  delay(100);
} 
