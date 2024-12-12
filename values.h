#ifndef VALUES_H
#define VALUES_H
struct params {
  uint8_t volume, sub, center, front, rear, bass, treble;
  bool mute, isdecoderenabled;
  String deco_2034;
  uint8_t input, sleepHrs,sleepMnts;
  

} p = { 0, 0, 0, 0, 0, 0, 0, 0,0, "1234", 0 ,0 ,0};

struct ctrl {
  char mode;
  bool setup;
  bool standby;
  bool showInput;
  bool settingsMode;
} c = { 0, 0, 0, 1 };
#endif
//sdklk