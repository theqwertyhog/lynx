#ifndef LynxMQTT_h
#define LynxMQTT_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class LynxMQTT {
  public:
    LynxMQTT();
    void connect();
    PubSubClient client;

  private:
    WiFiClientSecure wifiClient;
    static void callback(char* topic, byte* payload, unsigned int length);
};
#endif
