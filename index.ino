#include "LOUD.h"

bool send = false;
String ssid = "", pswd = "";
void setup() {
  Serial.begin(115200);
  ep.init();
 // myBt.init();
  allSetup();
  ep_read();
  if (wifi.isWifiConfigured) {
    read_wifi_ep();
    wifi.ssid = ssid;
    wifi.password = pswd;
    updateValsFromApp();
  }
}
volatile bool wifiRecvd = false;
bool settedTime = false;
void loop() {
  //Serial.println("loop");
  if (c.setup)
    ConfigWiFi();
  if (wifi.isWifiConfigured) {
    wifi.connect();
    wifi.onStatusChange(wifiStatus);
    wifi.loop();
  }
  enc.onchanged(forward, backward);
  enc.onButtonShort(shortPress);
  enc.onButtonLong(longPress, 1);
  ir.onRecv(Remote);
  myBt.getDataFromBt();
  showDisplay();
  // setAudio();
  if (!wifi.isWifiConfigured) send = 0;
  write_ep();
  hold();
  if (p.sleepHrs > 0 || p.sleepMnts > 0) {
    if (!settedTime) {
      slp.setTimeout(Time(p.sleepHrs, p.sleepMnts, 0));
      // Serial.printf("Schedule startedd %d : %d\n", p.sleepHrs, p.sleepMnts);
      settedTime = true;
      slp.reset();
    }
    slp.run();
  }
}

void ConfigWiFi() {
  wifi.config();
  while (wifi.configuring) {
    wifiRecvd = false;
    server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request) {
      String receivedName;
      String receivedData;
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        receivedName = p->name();
        receivedData = p->value();
        saveWifi(receivedName, receivedData);
        wifiRecvd = true;
      }
      request->send(200, "text/plain", "Data received: " + receivedName);
    });
    server.begin();
    while (wifi.configuring) {
      ir.onRecv([](int i) {
        if (i == PWR_SW) ESP.restart();
      });
      if (wifiRecvd) {
        //Serial.println("Data recvd");
        save_wifi_ep();
        ep.write(19, 1);
        ESP.restart();
        wifiRecvd = false;
      }
    }
  }
}

void saveWifi(String f, String d) {
  if (f.startsWith("ssid")) ssid = d;
  if (f.startsWith("pswd")) pswd = d;
  // Serial.println(ssid + " : " + pswd);
}
void save_wifi_ep() {
  ep.writeString(20, ssid);
  ep.writeString(ssid.length() + 21, pswd);
}
void read_wifi_ep() {
  ssid = ep.readString(20);
  pswd = ep.readString(ssid.length() + 21);
  Serial.println(ssid + "," + pswd);
}

void wifiStatus(bool w) {
  dis.wifistatus = w;
  Serial.println(WiFi.localIP());
}

uint8_t up[10] = { 0 };
void write_ep() {
  if (up[0] != p.volume) {
    up[0] = p.volume;
    wifi.sendVals("v", p.volume, send);
    ep.write(0, p.volume);
    setAudio();
  }

  if (up[1] != p.sub) {
    up[1] = p.sub;
    wifi.sendVals("s", p.sub, send);
    ep.write(1, p.sub);
    setAudio();
  }
  if (up[2] != p.center) {
    up[2] = p.center;
    wifi.sendVals("c", p.center, send);
    ep.write(2, p.center);
    setAudio();
  }
  if (up[3] != p.front) {
    up[3] = p.front;
    wifi.sendVals("f", p.front, send);
    ep.write(3, p.front);
    setAudio();
  }
  if (up[4] != p.rear) {
    up[4] = p.rear;
    wifi.sendVals("r", p.rear, send);
    ep.write(4, p.rear);
    setAudio();
  }
  if (up[5] != p.bass) {
    up[5] = p.bass;
    wifi.sendVals("b", p.bass, send);
    ep.write(5, p.bass);
    setAudio();
  }
  if (up[6] != p.treble) {
    up[6] = p.treble;
    wifi.sendVals("t", p.treble, send);
    ep.write(6, p.treble);
    setAudio();
  }
  if (up[7] != p.input) {
    up[7] = p.input;
    wifi.sendVals("i", p.input, send);
    ep.write(7, p.input);
    setAudio();
  }
}


void ep_read() {
  p.volume = ep.read(0);
  p.sub = ep.read(1);
  p.center = ep.read(2);
  p.front = ep.read(3);
  p.rear = ep.read(4);
  p.bass = ep.read(5);
  p.treble = ep.read(6);
  p.input = ep.read(7);
  wifi.isWifiConfigured = ep.read(19);
}

void sendBulkToAPP() {
  wifi.sendVals("v", p.volume);
  wifi.sendVals("s", p.sub);
  wifi.sendVals("c", p.center);
  wifi.sendVals("f", p.front);
  wifi.sendVals("r", p.rear);
  wifi.sendVals("b", p.bass);
  wifi.sendVals("t", p.treble);
}
void updateValsFromApp() {
  wifi.events([](uint8_t num, WStype_t type, uint8_t *payload, size_t size) {
    if (type == WStype_CONNECTED) {
      send = false;
      Serial.println("Connected to app");
      webSocket.broadcastTXT("connected");
      sendBulkToAPP();
    }
    if (type == WStype_TEXT) {
      send = false;
      String val = (char *)payload;
      int v = val.substring(1).toInt();
      Serial.println("Data " + val);

      if (val.startsWith("v")) {
        hold_reset();
        c.mode = 0;
        p.volume = v;
      }
      if (val.startsWith("f")) {
        hold_reset();
        c.mode = 3;
        p.front = v;
      }
      if (val.startsWith("r")) {
        hold_reset();
        c.mode = 4;
        p.rear = v;
      }
      if (val.startsWith("c")) {
        hold_reset();
        c.mode = 2;
        p.center = v;
      }
      if (val.startsWith("s")) {
        hold_reset();
        c.mode = 1;
        p.sub = v;
      }
      if (val.startsWith("b")) {
        hold_reset();
        c.mode = 5;
        p.bass = v;
      }
      if (val.startsWith("t")) {
        hold_reset();
        c.mode = 6;
        p.treble = v;
      }
      if (val.startsWith("m")) {
        p.mute = v;
        dis._menu = -1;
        dis.num = -1;
      }
      if (val.startsWith("i")) {
        p.input = v;
        c.showInput = 1;
        dis._in = -1;
      }
      if (val.startsWith("p")) {
        c.standby = v;
        dis._menu = -1;
        dis.num = -1;
        dis._in = -1;
        c.mode = 0;
      }
      if (val.startsWith("d")) {
        p.isdecoderenabled = val;
      }
      if (val.startsWith("T")) {
        settedTime = false;
        //Serial.println("Timer "+val);
        uint8_t inx = val.indexOf(":");
        p.sleepHrs = val.substring(1, inx).toInt();
        p.sleepMnts = val.substring(inx + 1).toInt();
      }
    }
  });
}
void setAudio() {
  audio.Slot1(
    p.input == 1 || p.input == 2 ? 1
    : p.input == 0               ? 0
                                 : 4,
    0,
    0,
    p.input > 2 ? 1 : 0,
    15 - p.bass,
    15 - p.treble,
    0);
  // 5.1 => in=4, rl=1;
  // 1 => in=0;
  // 2 => in=1;
  //without decoder
  //usb bt <= in0
  //aux <= in3
  //5.1
  //with decoder
  //5.1
  audio.Slot2(7, getVol(p.front), 7, getVol(p.front));
  audio.Slot3(7, getVol(p.sub), 7, getVol(p.sub));
  audio.Slot4(7, getVol(p.rear), 7, getVol(p.rear));
  
}

uint8_t getVol(uint8_t vol) {
  uint8_t ch_vol = vol + p.volume;
  if (ch_vol > 99) ch_vol = 99;
  ch_vol=99-ch_vol;
  if (ch_vol < 0) ch_vol = 0;
  return ch_vol;
}
uint64_t hld;
char tick;
void hold() {
  if (millis() - hld >= 1000) {
    hld = millis();
    tick++;
    if (tick > 3) {
      c.mode = 0;
      c.showInput = 1;
      dis._in = -1;
      tick = 5;
    }
  }
}
void hold_reset() {
  tick = 0;
  c.showInput = 0;
  dis._menu = -1;
}
void showDisplay() {
  char m = c.mode;

  dis.printTop(myBt.FileName);

  if (!myBt.play) {
    dis.printplay(1);
  }


  if (c.setup && c.standby != 1) {
    dis.printSetup(/*"        Data\n      recieved\n   Restarting in 5sec", 1*/);
  }

  if (c.standby) {
    dis.printStb();
  }
  if (c.settingsMode)
    dis.settingsMenu();

  if (c.setup != 1 && c.settingsMode != 1 && c.standby != 1 && myBt.play != 0) {
    dis.printWifiConnected();
    if (!p.mute) {
      if (c.showInput) dis.printInput(p.input);
      else dis.printFront(m);
      dis.print(
        m == 0 ? p.volume : m == 1 ? p.sub
                          : m == 2 ? p.center
                          : m == 3 ? p.front
                          : m == 4 ? p.rear
                          : m == 5 ? p.bass
                                   : p.treble);
    }
    dis.printMute(p.mute);
  }
}


void forward() {
  send = true;
  hold_reset();
  switch (c.mode) {
    case 0:
      if (p.volume <= 98) p.volume++;
      break;

    case 1:
      if (p.sub <= 98) p.sub++;
      break;

    case 2:
      if (p.center <= 50) p.center++;
      break;

    case 3:
      if (p.front <= 98) p.front++;
      break;

    case 4:
      if (p.rear <= 98) p.rear++;
      break;

    case 5:
      if (p.bass <= 14) p.bass++;
      break;

    case 6:
      if (p.treble <= 14) p.treble++;
      break;
  };
}



void backward() {
  send = true;
  hold_reset();
  switch (c.mode) {
    case 0:
      if (p.volume >= 1) p.volume--;
      break;

    case 1:
      if (p.sub >= 1) p.sub--;
      break;

    case 2:
      if (p.center >= 1) p.center--;
      break;

    case 3:
      if (p.front >= 1) p.front--;
      break;

    case 4:
      if (p.rear >= 1) p.rear--;
      break;

    case 5:
      if (p.bass >= 1) p.bass--;
      break;

    case 6:
      if (p.treble >= 1) p.treble--;
      break;
  };
}


void shortPress() {
  c.mode++;
  hold_reset();
  if (c.mode > 6) c.mode = 0;
}


void longPress() {
  send = true;
  p.input++;
  if (p.input > 6) p.input = 0;
  c.showInput = 1;
  dis._in = -1;
}

char _index = 0;
void Remote(int add) {
  //Serial.println(add, HEX);
  send = true;
  switch (add) {
    case VOL_PLUS:
      c.mode = 0;
      hold_reset();
      if (p.volume < 50) p.volume++;
      break;

    case VOL_MINU:
      c.mode = 0;
      hold_reset();
      if (p.volume >= 1) p.volume--;
      break;

    case SW_PLUS:
      c.mode = 1;
      hold_reset();
      if (p.sub < 50) p.sub++;
      break;

    case SW_MINU:
      c.mode = 1;
      hold_reset();
      if (p.sub >= 1) p.sub--;
      break;

    case CEN_PLUS:
      c.mode = 2;
      hold_reset();
      if (p.center < 50) p.center++;
      break;

    case CEN_MINU:
      c.mode = 2;
      hold_reset();
      if (p.center >= 1) p.center--;
      break;

    case FR_PLUS:
      c.mode = 4;
      hold_reset();
      if (p.rear < 50) p.rear++;
      break;

    case FR_MINU:
      c.mode = 4;
      hold_reset();
      if (p.rear >= 1) p.rear--;
      break;

    case FL_MINU:
      c.mode = 3;
      hold_reset();
      if (p.front >= 1) p.front--;
      break;

    case FL_PLUS:
      c.mode = 3;
      hold_reset();
      if (p.front < 50) p.front++;
      break;

    case SLEEP_MIN:
      p.mute = !p.mute;
      dis._menu = -1;
      dis.num = -1;
      delay(300);
      //Serial.printf("Mute: %d",p.mute);
      break;

    case SL_PLUS:
      c.mode = 5;
      hold_reset();
      if (p.bass < 15) p.bass++;
      break;

    case SL_MINU:
      c.mode = 5;
      hold_reset();
      if (p.bass >= 1) p.bass--;
      break;

    case SR_PLUS:
      c.mode = 6;
      hold_reset();
      if (p.treble < 15) p.treble++;
      break;

    case SR_MINU:
      c.mode = 6;
      hold_reset();
      if (p.treble >= 1) p.treble--;
      break;

    case SLEEP_PLU:
      c.setup = !c.setup;
      dis._menu = -1;
      dis.num = -1;
      dis._in = -1;
      delay(300);
      break;

    case PWR_SW:
      c.standby = !c.standby;
      dis._menu = -1;
      dis.num = -1;
      dis._in = -1;
      c.mode = 0;
      delay(300);
      break;

    case USB_R:
      p.input = 1;
      c.showInput = 1;
      dis._in = -1;
      break;

    case SD_FM:
      p.input = 2;
      c.showInput = 1;
      dis._in = -1;
      break;

    case AUX:
      p.input = 0;
      c.showInput = 1;
      dis._in = -1;
      break;

    case PRO:
      if (!p.isdecoderenabled) {
        p.input = 3;
        c.showInput = 1;
        dis._in = -1;
      }
      break;

    case DVD:
      if (p.isdecoderenabled) {
        p.input = 4;
        c.showInput = 1;
        dis._in = -1;
      }
      break;

    case AUTO:
      if (p.isdecoderenabled) {
        p.input = 5;
        c.showInput = 1;
        dis._in = -1;
      }
      break;

    case SAVE:
      if (p.isdecoderenabled) {
        p.input = 6;
        c.showInput = 1;
        dis._in = -1;
      }
      break;
    case TWO:
      if (c.standby) p.deco_2034 = "2";

      delay(300);
      break;
    case ZERO:
      if (c.standby) p.deco_2034 += "0";

      delay(300);
      break;
    case THREE:
      if (c.standby) p.deco_2034 += "3";

      delay(300);
      break;
    case FOUR:
      if (c.standby) {
        p.deco_2034 += "4";
        if (p.deco_2034.startsWith("2034")) {
          p.isdecoderenabled = !p.isdecoderenabled;
          dis.display2.clearDisplay();
          dis.display2.setCursor(3, 10);
          dis.display2.setTextSize(2);
          dis.display2.print(p.isdecoderenabled ? "DECODER\n  ENEABLED" : "DECODER\n  DISABLED");
          dis.display2.display();
          delay(2000);
          c.standby = 0;
          dis._in = -1;
          p.input = 0;
        }
      }
      delay(300);

      break;

    case CH_PLUS:
      myBt.nextTrack();
      delay(300);
      break;

    case CH_MINU:
    if(c.settingsMode){
      dis.onBackSetting();
    }else{
      myBt.prevTrack();
    }
      delay(300);
      break;

    case PLAY_PAUS:
      if (c.settingsMode) {
        dis.onOkSetting();
      } else {
        myBt.play_pause();
        if (myBt.play) dis.printplay(0);
        dis.num = -1;
      }
      delay(300);
      break;

    case RESET:
      c.settingsMode = !c.settingsMode;
      dis.clear();
      dis.num=-1;
      delay(300);
      break;

    case TUNE_PLUS:
      if (c.settingsMode) {
        dis.onUpSetting();
        delay(300);
      }
      break;

    case TUNE_MINU:
      if (c.settingsMode) {
        dis.onDownSetting();
        delay(300);
      }
      break;
  };
}