#ifndef LynxWifi_h
#define LynxWifi_h

#include <ESP8266WiFi.h>

class LynxWifi {
  public:
    LynxWifi();
    
    bool startSTA();
    bool stopSTA();
    bool startAP();
    bool stopAP();

  private:
    static void event(WiFiEvent_t e);
};

#endif

