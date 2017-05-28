#include <ESP8266WiFi.h>

#include "LynxConfig.h"
#include "LynxWifi.h"
#include "Lynx.h"

LynxWifi::LynxWifi() {
  DEBUG_MSG("Starting Wifi");

  WiFi.persistent(false);

  DEBUG_MSG(Lynx::config.wifiMode);
  if (strcmp(Lynx::config.wifiMode, "STA") == 0) {
    startSTA();
  } else if (strcmp(Lynx::config.wifiMode, "AP") == 0) {
    startAP();
  }
}

bool LynxWifi::startSTA() {
  DEBUG_MSG("Starting STA mode");

  WiFi.begin(Lynx::config.wifiSTASSID, Lynx::config.wifiSTAPass);
  WiFi.mode(WIFI_STA);
    
  return true;
}

bool LynxWifi::stopSTA() {
  return WiFi.disconnect(false);
}

bool LynxWifi::startAP() {
  DEBUG_MSG("Starting AP mode");
  DEBUG_MSG(Lynx::config.hostname);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(Lynx::config.hostname, Lynx::config.wifiAPPass);
}

bool LynxWifi::stopAP() {
  return WiFi.softAPdisconnect(false);
}

