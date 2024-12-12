class BT201 {

String arippa(String input) {
  String output = "";
  for (int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);
    if ((c >= 0x21 && c <= 0x7d) || c == '\n' || c==' ') {
      output += c;
    }
  }
  output = removeSubstring(output,"%23");
  return output;
}
String removeSubstring(String input, String substring) {
  input.replace(substring, "");
  return input;
}

public:

  String BufferData, FileName;
  void init() {
    Serial.begin(115200);
  }
  void getDataFromBt() {
    if (Serial.available()) {
      BufferData = arippa(Serial.readString());
      FileName = BufferData.substring(BufferData.indexOf("MF+") + 3);
    }
  }

  void send(String cmd, char t = 0) {
    if (t == 0) Serial.print("AT+" + cmd + "\r\n");
    else {
      Serial.print("AT+" + cmd + "[\r]\n");
    }
  }

  void setName(String name) {
    send("BD" + name);
  }

  void nextTrack() {
    send("CC");
  }

  void prevTrack() {
    send("CD");
  }
  bool play = 1;
  void play_pause() {
    play=!play;
    send(play?"AA01":"AA02");
  }

  void switchMode(char m) {
    if (m == 'B') send("CM01");
    if (m == 'U') send("CM02");
    if (m == 'S') send("CM03");
    if (m == 'A') send("CM05");
  }
}myBt;