#ifndef LOUD_H
#define LOUD_H

#include "myEncoder.h"
#include "myRemote.h"
#include "Display.h"
#include "myTimer.h"
#include "myWiFI.h"
#include "values.h"
#include "BT201.h"

//======================eeprom=========================
#include "eprom.h"

//===================R2S pins===============
#define DATA 14  // R2s
#define CLK 12  //  R2s
#include "audio.h"

//====================Encoder=================
#define ENCODER_PIN_A 25
#define ENCODER_PIN_B 26
#define ENCODER_PIN_SW 33
myEncoder enc(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PIN_SW);

void IRAM_ATTR readEncoderISRA() {
  enc.readIsrA();
}
void IRAM_ATTR readEncoderISRB() {
  enc.readIsrB();
}
//====================Remote==========================
#define IR_PIN 27
#define IR_OUT_PIN 18
myRemote ir;

//===================================== DIsplay=====================
myDisplay dis;

//===============================Sleeper=======================
MyTimer slp, timer;
bool shuttedDown = false;
void allSetup() {
  pinMode(DATA,OUTPUT);
  pinMode(CLK,OUTPUT);

  enc.begin(readEncoderISRA, readEncoderISRB);
  ir.init(IR_PIN,IR_OUT_PIN);
  dis.begin();
  dis.printLogo();
  delay(3000);
  dis.clear();
  slp.setCallback([]() { 
    
    Serial.println("Shutted down");
      c.standby = 1;
      dis._menu = -1;
      dis.num = -1;
      dis._in = -1;
      c.mode = 0;
      c.setup = 0;
     
       });
  slp.setOnTickCallback([](long unsigned int tick){
    Serial.println(tick);
  });


  p = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
}
#endif