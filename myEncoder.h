#include "MyButton.h"
class myEncoder {
  int val;
  char A, B, Bbt;
  volatile int a, b, u, y;
public:
  Button btn;

  myEncoder(char pina, char pinb, char bt) {
    A = pina;
    B = pinb;
    Bbt = bt;
  }

  void begin(void (*isrA)(), void (*isrB)()) {
    pinMode(A, INPUT_PULLUP);
    pinMode(B, INPUT_PULLUP);
    btn.init(Bbt, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(A), isrA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(B), isrB, CHANGE);
  }

  void readIsrA() {
    if (digitalRead(A) != a) {
      a = digitalRead(A);
      if (digitalRead(A) == 1 && digitalRead(B) == 0) {
        u = 1;
        y = 0;
      }
    }
  }
  void readIsrB() {
    if (digitalRead(B) != b) {
      b = digitalRead(B);
      if (digitalRead(B) == 1 && digitalRead(A) == 0) {
        u = 0;
        y = 1;
      }
    }
  }
  void onchanged(void (*f)(), void (*bc)()) {
    if (u == 1 && y == 0) f();
    if (u == 0 && y == 1) bc();
    u = 0;
    y = 0;
  }
  void onButtonShort(void (*f)()) {
    btn.ShortPress(f);
  }
  void onButtonLong(void (*f)(), int d) {
    btn.longPress(f, d);
  }
};