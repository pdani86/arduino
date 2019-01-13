#include <SPI.h>
#include <RF24.h>

#define RF24_CE_PIN 7
#define RF24_CS_PIN 8
#define RADIO_CHANNEL 29
#define RC_ADDRESS 0x0086102400

RF24 radio(RF24_CE_PIN,RF24_CS_PIN);

void setup_radio() {
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  //radio.setDataRate(RF24_250KBPS);
  //radio.setAutoAck(false);
  radio.setChannel(RADIO_CHANNEL);
  //radio.openReadingPipe(1,RC_ADDRESS);
  //radio.startListening();
  radio.openWritingPipe(RC_ADDRESS);
  radio.stopListening();
}

void setup() {
  //Serial.begin(9600);
  Serial.begin(115200);
  while(!Serial);
  setup_radio();
  Serial.println("INIT");
}

struct RADIO_CONTROL_PACKET
{
  char steer;
  byte throttle;
  byte brake;
  byte clutch;
  char gear;
  byte lamps;

  inline RADIO_CONTROL_PACKET() {memset(this,0,sizeof(this));}
};

enum E_SERIAL_PARSE_STATE
{
  IDLING,
  RECEIVING_CONTROLLER_STATE,
  RECEIVING_CMD,
  UNKNOWN_STATE,
  
};

static void myDecode(const byte* src,int srclen,byte* dst) {
    for(int i=0;i<srclen/2;i++) {
        dst[i] = (src[i*2]>>4) | (src[i*2+1]&0xf0);
    }
}

class SerialHandler
{
  public:
  RADIO_CONTROL_PACKET controlPacket;
  bool newDataFlag;
  
  E_SERIAL_PARSE_STATE serialParseState;

  inline SerialHandler() {reset();}

  inline void reset() {
    memset(controller_state_packet_buf,0,sizeof(controller_state_packet_buf));
    serialParseState = IDLING;
    packet_pos = 0;
  }
  
  void handleSerial();
  
private:
  int packet_pos;
  byte controller_state_packet_buf[64];
  
  
};

void SerialHandler::handleSerial() {
   while(Serial.available())
   {
     //byte ch = Serial.peek();
     byte ch = Serial.read();
     //Serial.print("Serial char: "); Serial.println((byte)ch,HEX);
     switch(serialParseState) {
      case IDLING:
        if(ch=='s') {
          serialParseState = RECEIVING_CONTROLLER_STATE;
          packet_pos = 0;
        }
      break;
      /*case RECEIVING_CMD:
        if(ch==' ') {
          serialParseState = RECEIVING_CONTROLLER_STATE;
        } else {
          serialParseState = IDLING;
        }
      break;*/
      case RECEIVING_CONTROLLER_STATE:
        if(((ch&0x0f)==0) && packet_pos<64 ) {
          controller_state_packet_buf[packet_pos] = ch;
          packet_pos++;
          //Serial.print("packet_pos - ");
          //Serial.println(packet_pos);
        } else {
          //decode
          myDecode((byte*)controller_state_packet_buf,packet_pos,(byte*)&controlPacket);
          serialParseState = IDLING;
          packet_pos = 0;
          newDataFlag = true;
          //Serial.print("steer - ");
          //Serial.println((int)controlPacket.steer);
        }
      break;
      case UNKNOWN_STATE:
      break;
     }
   }
}

SerialHandler serialHandler;


void sendControllerStatePacket() {
  radio.write(&serialHandler.controlPacket,sizeof(serialHandler.controlPacket));
}

unsigned long packet_send_interval_ms = 100;
unsigned long last_packet_sent_time_ms = 0;

void loop() {
  serialHandler.handleSerial();
  unsigned long now = millis();
  if(now-last_packet_sent_time_ms >= packet_send_interval_ms) {
    sendControllerStatePacket();
    last_packet_sent_time_ms = now;
    serialHandler.newDataFlag = false;
  }
}
