#ifndef Display_h
#define Display_h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Font.h"

#define OLED1_ADDRESS 0x3C
#define OLED2_ADDRESS 0x3C


class myDisplay {
private:

  char pos;

  bool scrlld = 1;
  String _str;
  bool _ws, _mute;
  uint64_t _dis_timer;
  int tck;

  void printNum(char, char, char);
  void scroll();
  void settingsRight(String name, uint8_t index, String menu1, String menu2 = "", String menu3 = "");
  void settingsLeft(String name, uint8_t index, String menu1, String menu2 = "", String menu3 = "");


public:
  char _menu = -1, num;
  char _in = -1;
  bool btstatus, wifistatus = 0;
  uint8_t leftIndex = 0, rightIndex = 0, sleepTime = 20;
  bool selection = 0, wakeonapp = true, wifion = true, decoderenabled = true;

  Adafruit_SSD1306 display1 = Adafruit_SSD1306(128, 64, &Wire, -1);
  Adafruit_SSD1306 display2 = Adafruit_SSD1306(128, 64, &Wire1, -1);
  void begin();
  void printLogo();
  void printFront(char);
  void print(char);
  void printInput(char);
  void clearBig1();
  void clearBig2();
  void printplay(bool p);
  void printTop(String name);
  void printMute(bool);
  void printStb();
  void printSetup(String dat = "   WiFi\nSETUP MODE", char size = 2);
  void printWifiConnected();
  void clear(bool d1 = 1, bool d2 = 1);
  void settingsMenu();
  void onDownSetting();
  void onUpSetting();
  void onBackSetting();
  void onOkSetting();
};


void myDisplay::printLogo() {
  display1.drawBitmap(0, 1, LOUD_LO, 128, 64, WHITE);
  display2.drawBitmap(0, 1, LOUD_UD, 128, 64, WHITE);
  display1.display();
  display2.display();
}
void myDisplay::printFront(char i) {
  if (_menu != i) {
    _menu = i;
    display1.clearDisplay();
    display1.drawBitmap(0, 20,
                        i == 0 ? VOL : i == 1 ? Subwfr
                                     : i == 2 ? Center
                                     : i == 3 ? Front
                                     : i == 4 ? Rear
                                     : i == 5 ? Bass
                                              : Treb,
                        128, 50, WHITE);
    display1.display();
  }
}

void myDisplay::printInput(char i) {
  if (i != _in) {
    _in = i;
    display1.clearDisplay();
    display1.drawBitmap(0, 15, inallArray[i], 128, 50, WHITE);
    display1.display();
  }
}

void myDisplay::begin() {
  Wire.begin(15, 13);
  //Wire.setPins(int sda, int scl)
  Wire1.begin(16, 4);


  display1.begin(SSD1306_SWITCHCAPVCC, OLED1_ADDRESS);
  display2.begin(SSD1306_SWITCHCAPVCC, OLED2_ADDRESS);
  display1.clearDisplay();
  display2.clearDisplay();
}

void myDisplay::clear(bool d1, bool d2) {
  if (d1) display1.clearDisplay();
  if (d2) display2.clearDisplay();

  display1.display();
  display2.display();
}


void myDisplay::clearBig1() {
  display1.fillRect(0, 20, 128, 64, SSD1306_BLACK);
  display1.display();
}
void myDisplay::clearBig2() {
  display2.fillRect(0, 0, 128, 128, SSD1306_BLACK);
  display2.display();
}


void myDisplay::printNum(char n, char x, char y) {
  display2.drawBitmap(x, y,
                      n == 0 ? num0 : n == 1 ? num1
                                    : n == 2 ? num2
                                    : n == 3 ? num3
                                    : n == 4 ? num4
                                    : n == 5 ? num5
                                    : n == 6 ? num6
                                    : n == 7 ? num7
                                    : n == 8 ? num8
                                             : num9,
                      50, 49, WHITE);
}
void myDisplay::print(char i) {
  if (num != i) {
    num = i;
    display2.fillRect(0, 15, 128, 64, SSD1306_BLACK);
  }
  printNum(i / 10, (i / 10) == 1 ? 25 : 10, 15);
  printNum(i % 10, 65, 15);
  display2.display();
}

void myDisplay::scroll() {
  if (millis() - _dis_timer >= 100) {
    _dis_timer = millis();
    tck++;
    if (tck > 10) {
      tck = 11;
      pos++;
    }
  }
}
void myDisplay::printTop(String str) {
  if (_str != str) {
    _str = str;
    scrlld = 0;
    tck = 0;
  }
  if (!scrlld) {
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.fillRect(0, 0, 128, 12, BLACK);
    display1.setCursor(0, 1);
    display1.print(str.substring(pos, pos + 20));
    scroll();
    if (pos > str.length()) {
      pos = 0;
      display1.fillRect(0, 0, 128, 10, BLACK);
      scrlld = 1;
    }
  }
  display1.display();
}
void myDisplay::printMute(bool m) {
  if (_mute != m) {
    _mute = m;

    if (m) {
      display1.clearDisplay();
      display2.clearDisplay();
      display1.drawBitmap(35, 15, Mute, 49, 49, WHITE);

      display2.setTextSize(4);
      display2.setTextColor(SSD1306_WHITE);
      display2.setCursor(15, 20);
      display2.print("MUTE");
      display2.display();
      display1.display();
    }
  }
}
void myDisplay::printStb() {
  display1.clearDisplay();
  display2.clearDisplay();

  display1.drawBitmap(33, 0, power, 64, 64, WHITE);

  display2.setTextSize(4);
  display2.setTextColor(SSD1306_WHITE);
  display2.setCursor(5, 20);
  display2.print("ST-BY");

  display2.display();
  display1.display();
}
void myDisplay::printplay(bool p) {
  display2.fillRect(0, 15, 128, 64, SSD1306_BLACK);
  display2.setCursor(p ? 14 : 22, 30);
  display2.setTextColor(SSD1306_WHITE);
  display2.setTextSize(3);
  display2.print(p ? "PAUSE" : "PLAY");
  display2.display();
  if (!p) delay(1000);
}
void myDisplay::printSetup(String dat, char size) {
  display1.clearDisplay();
  display2.clearDisplay();

  display1.drawBitmap(25, 0, Setup, 64, 64, WHITE);

  display2.setTextSize(size);
  display2.setTextColor(SSD1306_WHITE);
  display2.setCursor(0, 20);
  display2.print(dat);

  display2.display();
  display1.display();
}

void myDisplay::printWifiConnected() {
  if (wifistatus != _ws) {
    display2.fillRect(0, 0, 128, 14, BLACK);
    _ws = wifistatus;
    if (wifistatus) {
      display1.clearDisplay();
      display2.clearDisplay();
      display1.drawBitmap(25, 0, WIFI_CONN, 64, 64, WHITE);
      display2.setTextSize(2);
      display2.setTextColor(SSD1306_WHITE);
      display2.setCursor(0, 15);
      display2.print("    WiFi \n Connected");
      display2.display();
      display1.display();
      delay(2000);
      display1.clearDisplay();
      display2.clearDisplay();
      display2.display();
      display1.display();
    }
  }
  if (!wifistatus) {
    display2.setTextSize(1);
    display2.setTextColor(SSD1306_WHITE);
    display2.setCursor(80, 1);
    display2.print("NO WiFi");
    display2.display();
  }
}
#define displayMenu(t, b) "Sleep < " + (t > 0 ? (String(t) + " S") : "Never") + " > ", b ? "Wake on App: Yes" : "Wake on App: NO", "Reset to Deafult"
#define audioMenu(b) b ? "Decoder mode: ON" : "Decoder mode: Off", "Reset tone"
#define wifiMenu(b) b ? "WiFi: ON" : "WiFi: Off", "Configure WiFi"

void myDisplay::settingsMenu() {
  settingsLeft("SETTINGS", leftIndex, "Display settings", "Audio Settings", "WiFi Settings");
  if (leftIndex == 0)
    settingsRight("DISPLAY", selection ? rightIndex : 10, displayMenu(sleepTime, wakeonapp));

  if (leftIndex == 1)
    settingsRight("AUDIO", selection ? rightIndex : 10, audioMenu(decoderenabled));

  if (leftIndex == 2)
    settingsRight("WiFi", selection ? rightIndex : 10, wifiMenu(wifion));
}

void myDisplay::settingsLeft(String name, uint8_t index, String menu1, String menu2, String menu3) {
  display1.clearDisplay();
  display1.fillRoundRect(0, 0, 127, 10, 3, 1);
  display1.drawRect(0, 0, 127, 63, 1);
  display1.setCursor(40, 1);
  display1.setTextColor(BLACK, WHITE);
  display1.setTextSize(1);
  display1.print(name);
  display1.setTextColor(WHITE);
  
  int len = 1;

  display1.setCursor(10, 15);
  display1.print(menu1);

  if (menu2 != "") {
    display1.setCursor(10, 30);
    display1.print(menu2);
    len = 2;
  }
  if (menu3 != "") {
    display1.setCursor(10, 45);
    display1.print(menu3);
    len = 3;
  }
  if (len > 1) {
    display1.drawRoundRect(5, index == 0 ? 12 : index == 1 ? 27
                                                           : 42,
                           110, 15, 3, 1);
  }
  display1.display();
}
void myDisplay::settingsRight(String name, uint8_t index, String menu1, String menu2, String menu3) {
  display2.clearDisplay();
  display2.fillRoundRect(0, 0, 127, 10, 3, 1);
  display2.drawRect(0, 0, 127, 63, 1);
  display2.setCursor(40, 1);
  display2.setTextColor(BLACK, WHITE);
  display2.setTextSize(1);
  display2.print(name);
  display2.setTextColor(WHITE);
  int len = 1;

  display2.setCursor(10, 15);
  display2.print(menu1);

  if (menu2 != "") {
    display2.setCursor(10, 30);
    display2.print(menu2);
    len = 2;
  }
  if (menu3 != "") {
    display2.setCursor(10, 45);
    display2.print(menu3);
    len = 3;
  }
  if (len > 1 && index < 3) {
    display2.drawRoundRect(5, index == 0 ? 12 : index == 1 ? 27
                                                           : 42,
                           110, 15, 3, 1);
  }
  display2.display();
}
void myDisplay::onDownSetting() {
  if (selection == 0) {
    leftIndex++;
    if (leftIndex > 2) leftIndex = 0;
  } else {
    rightIndex++;
    if (rightIndex > 2) rightIndex = 0;
  }
}

void myDisplay::onUpSetting() {
  if (selection == 0) {
    leftIndex--;
    if (leftIndex < 0) leftIndex = 2;
  } else {
    rightIndex--;
    if (rightIndex < 0) rightIndex = 2;
  }
}

void myDisplay::onBackSetting() {
  selection = 0;
}

void myDisplay::onOkSetting() {
  if (selection && leftIndex == 0) {
    if (rightIndex == 0) {
      sleepTime += 5;
      if (sleepTime > 60) sleepTime = 0;
    }
    if (rightIndex == 1) wakeonapp = !wakeonapp;
    if (rightIndex == 2) {
      sleepTime = 20;
      wakeonapp = true;
    }
  }
  if (selection && leftIndex == 1) {
    if (rightIndex == 0) { decoderenabled = !decoderenabled; }
    if (rightIndex == 1) decoderenabled = false;
  }
  if (selection && leftIndex == 2) {
    if (rightIndex == 0) { wifion = !wifion; }
    if (rightIndex == 1) {}
  }
  if (!selection) selection = 1;
}
#endif
