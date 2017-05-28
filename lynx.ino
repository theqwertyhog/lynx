#include "Lynx.h"
#include <Ticker.h>
#include "mdns.h"

Lynx lynx;
Ticker mqttTicker;
bool mqttTicked = false;
Ticker resetTicker;
bool resetTicked = false;

void setup() {  
  DEBUG_MSG(Lynx::getDeviceInfo().c_str());
  lynx.initmDNS(lynx.config.hostname);
  Lynx::udp.begin(3000);
  mqttTicker.attach(5, mqttTickedCB);
  
  attachInterrupt(Lynx::BTN_READ_PIN, startResetTicker, HIGH);
}

void loop() {
 if (resetTicked) {
    resetTicked = false;
    if (digitalRead(Lynx::BTN_READ_PIN) == HIGH) {
      DEBUG_MSG(F("Reset requested"));
      lynx.mqtt.client.publish(lynx.config.messagesTopic, "W,RR"); // Warning: Reset requested
      lynx.config.reset();
      ESP.reset();
    } else {
      DEBUG_MSG(F("Reset abandoned"));
    }
  }
      
  if (!lynx.config.isDeviceEnabled) {
    DEBUG_MSG(F("Device is disabled!"));
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (mqttTicked) {
      mqttTicked = false;
      lynx.mqtt.connect();
    }
    
    lynx.udpLoop();

    if (lynx.mqtt.client.connected()) {
      lynx.mqtt.client.loop();
    }
  }
  
}

void mqttTickedCB() {
  mqttTicked = true;
}

void startResetTicker() {
  DEBUG_MSG(F("Reset initiated"));
  resetTicker.once(5, resetTickedCB);
}

void resetTickedCB() {
  resetTicked = true;
}

