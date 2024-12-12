#include "IPAddress.h"
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

WebSocketsServer webSocket = WebSocketsServer(81);
AsyncWebServer server(80);

class myWifi {
  bool flag=1;
  bool flag2 = 1;
  bool WiFiConnected = 0;

public:
  String ssid = "null";
  String password = "null";
  bool isWifiConfigured = false;
  char address = 8;
  bool _WiFiConnected = 0;
  bool configuring = false;


  bool config() {
    configuring = true;
    WiFi.disconnect();
    delay(500);
    WiFi.softAP("LOUD config wifi", "12345678");
    Serial.println(WiFi.softAPIP());
    return configuring;
  }


  void onStatusChange(void (*cb)(bool)) {
    if (WiFiConnected != _WiFiConnected) {
      _WiFiConnected = WiFiConnected;
      if (WiFiConnected) flag = 1;
      else flag2 = 1;
      cb(WiFiConnected);
    }
  }

  void connect() {
    if (ssid == "" || password == "") return;
    if (!WiFiConnected && flag) {
      WiFi.begin(ssid, password);
      flag = 0;
    }
    WiFiConnected = WiFi.isConnected();

    if (WiFiConnected && flag2) {
      if (!MDNS.begin("esp32")) {  // Set the hostname to "esp32.local"
        Serial.println("Error starting mDNS");
        return;
      }
      flag2 = 0;
      webSocket.begin();
    }
  }


  void loop() {
    webSocket.loop();
  }

  void events(void (*cb)(uint8_t, WStype_t, uint8_t *, size_t)) {
    webSocket.onEvent(cb);
  }

  void sendVals(String key, uint8_t val, bool s = true) {
    if (s) {
      webSocket.broadcastTXT(key + String(val));
      Serial.printf("%s %d send=%d\n", key, val, s);
    }
  }

} wifi;
