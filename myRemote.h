#include <IRremote.hpp>

#define PWR_SW 0xBE41FE01
#define MUTE_SD 0xBD42FE01
#define PRO 0xBF40FE01
#define USB_R 0xB946FE01
#define SD_FM 0xBA45FE01
#define AUX 0xBB44FE01
#define DVD 0xFB04FE01
#define AUTO 0xB54AFE01
#define SAVE 0xB748FE01
#define TUNE_PLUS 0xB649FE01
#define TUNE_MINU 0xF20DFE01
#define CH_PLUS 0xB34CFE01
#define CH_MINU 0xB14EFE01
#define PLAY_PAUS 0xB24DFE01
#define SLEEP_MIN 0xF00FFE01
#define SLEEP_PLU 0xF30CFE01
#define FL_MINU 0xFC03FE01
#define FL_PLUS 0xFD02FE01
#define FR_PLUS 0xFF00FE01
#define FR_MINU 0xFE01FE01
#define SW_PLUS 0xF40BFE01
#define SW_MINU 0xF807FE01
#define CEN_PLUS 0xF50AFE01
#define CEN_MINU 0xF906FE01
#define VOL_PLUS 0xF609FE01
#define VOL_MINU 0xFA05FE01
#define SL_MINU 0xEC13FE01
#define SL_PLUS 0xED12FE01
#define SR_MINU 0xEE11FE01
#define SR_PLUS 0xEF10FE01
#define ZERO 0xA659FE01
#define ONE 0xAD52FE01
#define TWO 0xAE51FE01
#define THREE 0xAF50FE01
#define FOUR 0xE817FE01
#define FIVE 0xE916FE01
#define SIX 0xEA15FE01
#define SEVEN 0xA956FE01
#define EIGHT 0xAA55FE01
#define NINE 0xAB54FE01
#define REPEAT 0xA55AFE01
#define RESET 0xA758FE01
#define EQ_MIN 0xE41BFE01
#define EQ_PLUS 0xE51AFE01
#define DISP_MIN 0xE619FE01
#define DISP_PLUS 0xE718FE01

class myRemote {
  uint8_t OUT_IR;
  IRRawDataType newKey = 0;
  int irLong = 0;
public:
  void init(char pin,char ir_out_pin) {
    OUT_IR=ir_out_pin;
    IrReceiver.begin(pin);
    pinMode(pin, INPUT_PULLUP);
    pinMode(ir_out_pin,OUTPUT);
    
  }

////----------------------DELAY------------------------------------//////
void Delay_1ms()
 {
 int j,k;
  for(j=0; j<1004; j++)
  for(k=0; k<39; k++)NOP();
 }
 void Delay_9ms()
 {
  uint8_t j;
  for(j=0; j<9; j++)
  {
   Delay_1ms();
  }NOP();
 }
 void Delay_40ms()
 {
  uint8_t j;
  for(j=0; j<41; j++)
  {
   Delay_1ms();
  }
 }
 void Delay_96ms()
 {
  uint8_t j;
  for(j=0; j<97; j++)
  {
   Delay_1ms();
  }
 }
 void Delay_4500us()
 {
 int j,k;
  for(j=0; j<4; j++){
  Delay_1ms();}

  for(j=0; j<1004; j++)
  for(k=0; k<20; k++)NOP();
  
 }
 void Delay_572us()
 {
 int j,k;
  
  for(j=0; j<1004; j++)
  for(k=0; k<22; k++)NOP();
 }
 void Delay_562us()
 {
 int j,k;
  
  for(j=0; j<990; j++)
  for(k=0; k<22; k++)NOP();
 }
 void Delay_552us()
 {
 int j,k;
  
  for(j=0; j<978; j++)
  for(k=0; k<22; k++)NOP();
 }
 void Delay_1685us()
 {
 int j,k;
  Delay_1ms();
  for(j=0; j<1030; j++)
  for(k=0; k<26; k++)NOP();
 }
 void Delay_2235us()
 {
 int j,k;
  Delay_1ms();
  Delay_1ms();
  for(j=0; j<350; j++)
  for(k=0; k<26; k++)NOP();
}
////---------------------------IR SEND--------------------------------------////
void send(uint32_t cde)
{
	uint8_t irl;
	
   digitalWrite(OUT_IR, LOW);
   Delay_9ms();
   digitalWrite(OUT_IR, HIGH);
   Delay_4500us();
    
  for(irl=0;irl<32;irl++)
   {	
        digitalWrite(OUT_IR, LOW);
        Delay_572us();
        digitalWrite(OUT_IR, HIGH);
		  if(cde & 0x01)
        {
         Delay_1685us();
        }
		  else 
        {
         Delay_572us();
        }
		   cde=cde>>1;     
   }
	digitalWrite(OUT_IR, LOW);
	Delay_562us();
	digitalWrite(OUT_IR, HIGH);
	Delay_40ms();
}

void repeat(void)
{
    digitalWrite(OUT_IR, LOW);
    Delay_9ms();
    digitalWrite(OUT_IR, HIGH);
    Delay_2235us();
    digitalWrite(OUT_IR, LOW);
    Delay_552us();
    digitalWrite(OUT_IR, HIGH);
    Delay_96ms(); 
}
////---------------------------IR RCV---------------------------------------////

  void onRecv(void cb(int)) {
    if (IrReceiver.decode()) {
      if (IrReceiver.decodedIRData.protocol == NEC)
        if (IrReceiver.decodedIRData.decodedRawData != 0) {
          newKey = IrReceiver.decodedIRData.decodedRawData;
          cb(newKey);
          irLong = 0;
        } else {
          irLong++;
          if (irLong > 3) {
            cb(newKey);
            irLong = 10;
          }
        }

      IrReceiver.resume();
    }
  }
};