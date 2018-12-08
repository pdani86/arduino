#include <SPI.h>
#include <IRremote.h>

#define DISPLAY_CS_PIN 8
const int IR_RECV_PIN = 4;

byte hexTo7seg[] = {
  B1111110,
  B0110000,
  B1101101,
  B1111001,
  B0110011,

  B1011011,
  B1011111,
  B1110000,
  B1111111,
  B1111011,

  B1110111,
  B0011111,
  B1001110,
  B0111101,
  B1001111,

  B1000111
};

class SpiDisplay8
{
  void writeReg(unsigned char addr,unsigned char val);

public:
  inline SpiDisplay8(byte csPin):csPin(csPin) {};
  inline void init();

  inline void test(bool on = true) {writeReg(0x0f,on?1:0);}
  inline void setDecodeMode(bool set=true) { writeReg(0x09,set?0xff:0x00); }
  inline void setDigit(byte ix,byte val) { writeReg(1+ix,val); }
  inline void setIntensity(byte val) {writeReg(0x0a,val); }
  inline void fill(byte val) {for(int i=0;i<8;i++) {writeReg(1+i,val);}};
  void setHexCode32(unsigned long code);

private:
  byte csPin;
};

void SpiDisplay8::setHexCode32(unsigned long code) {
  for(int i=0;i<4;i++) {
    byte curH = (code>>(i*8))&0xff;
    byte curL = curH&0x0f;
    curH = curH>>4;
    setDigit(2*i,hexTo7seg[curL]);
    setDigit(2*i+1,hexTo7seg[curH]);
  }
}

inline void SpiDisplay8::init() {
    pinMode(csPin,OUTPUT);
    setIntensity(0x0f); // max
    // scan all 8 characters
    writeReg(0x0b,0x07);
    // turn on normal mode
    writeReg(0x0c,0x01);
    // turn off display test
    test(false);
    setDecodeMode(false);
    for(int i=0;i<8;i++) writeReg(1+i,0x01); // clear display (---- ----)
 }

void SpiDisplay8::writeReg(unsigned char addr,unsigned char val) {
    digitalWrite(DISPLAY_CS_PIN,LOW);
    SPI.transfer(addr);
    SPI.transfer(val);
    digitalWrite(DISPLAY_CS_PIN,HIGH);
    //delay(1);
  }

SpiDisplay8 mydisplay(DISPLAY_CS_PIN);

IRrecv irrecv(IR_RECV_PIN);
decode_results ir_results;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
 
  pinMode(DISPLAY_CS_PIN,OUTPUT);
  digitalWrite(DISPLAY_CS_PIN,HIGH);

  SPI.begin();
  delay(10);
  mydisplay.init();
}

void loop() {
  unsigned long irValue = 0;
  if(irrecv.decode(&ir_results)) {
    mydisplay.setHexCode32(ir_results.value);
    Serial.println(ir_results.value,HEX);
    irrecv.resume();
  }
}

