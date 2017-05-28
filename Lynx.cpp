#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <ESP8266httpUpdate.h>

#include "Lynx.h"
#include "FS.h"

LynxConfig Lynx::config = LynxConfig();
LynxWifi Lynx::wifi = LynxWifi();
LynxCommands Lynx::commands = LynxCommands();
LynxMQTT Lynx::mqtt = LynxMQTT();

WiFiUDP Lynx::udp = WiFiUDP();

byte Lynx::applianceState = 0;

Lynx::Lynx() {
  initEEPROM();
  initPin();
  initSerial();
}

void Lynx::initSerial() {
  Serial.begin(115200);
  DEBUG_MSG(F("Booting..."));
}

void Lynx::initEEPROM() {
  DEBUG_MSG(F("Initializing EEPROM"));
  EEPROM.begin(512);
}

void Lynx::initPin() {
  pinMode(Lynx::BTN_READ_PIN, INPUT);
  pinMode(Lynx::APPLIANCE_WRITE_PIN, OUTPUT);

  Lynx::applianceState = getDefaultApplianceState();
  digitalWrite(Lynx::APPLIANCE_WRITE_PIN, applianceState);
}

void Lynx::initmDNS(char* hostname) {
  DEBUG_MSG(F("Starting MDNS"));
  if (!MDNS.begin(hostname)) {
    DEBUG_MSG(F("MDNS responder failed"));
  }

  DEBUG_MSG(F("MDNS responder started"));
  
  MDNS.addService("lynx", "udp", 3000);
  MDNS.addServiceTxt((const char*) "lynx", (const char*) "udp", (const char*) "id", (const char*) Lynx::config.deviceID);
  MDNS.addServiceTxt((const char*) "lynx", (const char*) "udp", (const char*) "type", (const char*) "sock");
}

byte Lynx::getDefaultApplianceState() {
  return EEPROM.read(1);
}

void Lynx::setDefaultApplianceState(byte state) {  
  EEPROM.write(1, state);
  EEPROM.commit();
}

byte Lynx::getApplianceState() {
  return Lynx::applianceState;
}

void Lynx::setApplianceState(byte state) {
  digitalWrite(Lynx::APPLIANCE_WRITE_PIN, state);
  Lynx::applianceState = state;
}

String Lynx::getDeviceStatus() {
  String status = String(Lynx::getApplianceState());
  status.concat(',');
  status.concat(Lynx::getDefaultApplianceState());
  status.concat(',');
  status.concat(ESP.getFreeHeap());
  status.concat(',');
  status.concat(WiFi.getMode());
  status.concat(',');
  status.concat(WiFi.SSID() ? WiFi.SSID() : "NULL");
  status.concat(',');
  status.concat(WiFi.RSSI());
  status.concat(',');

  return status;
}

String Lynx::getDeviceInfo() {
 String info = Lynx::config.getConfigStr();
 info.concat(ESP.getChipId());
 info.concat(',');
 info.concat(ESP.getFlashChipId());
 info.concat(',');
 info.concat(ESP.getFlashChipSize());
 info.concat(',');
 info.concat(ESP.getFlashChipRealSize());
 info.concat(',');
 info.concat(ESP.getFlashChipSpeed());
 info.concat(',');

 return info;
}


void Lynx::udpLoop() {
  int packetSize = Lynx::udp.parsePacket();
    if (packetSize) {
      char incomingPacket[packetSize];
      int len = Lynx::udp.read(incomingPacket, packetSize);
      if (len > 0) {
        incomingPacket[len] = 0;
      }
      DEBUG_MSG(incomingPacket);
      commands.handleCommand(incomingPacket, false, Lynx::udp.remoteIP(), Lynx::udp.remotePort());
    }
}

void Lynx::writeUDP(IPAddress ip, uint16_t port, const char* data) {
  Lynx::udp.beginPacket(ip, port);
  Lynx::udp.write(data);
  Lynx::udp.endPacket();
}

void Lynx::updateFirmware(char* hostname, int port, char* newFirmwareVersion, IPAddress callerIP, uint16_t callerPort) {
  t_httpUpdate_return ret = ESPhttpUpdate.update(hostname, port, "/lynx/get-firmware", Lynx::config.deviceID);
  switch(ret) {
      case HTTP_UPDATE_FAILED:
          DEBUG_MSG(F("[update] Update failed."));
          Lynx::writeUDP(callerIP, callerPort, commands.prepareResponse("M,FU,-1,").c_str());
          break;
      case HTTP_UPDATE_NO_UPDATES:
          DEBUG_MSG(F("[update] Update no Update."));
          Lynx::writeUDP(callerIP, callerPort, commands.prepareResponse("M,FU,0,").c_str());
          break;
      case HTTP_UPDATE_OK:
          DEBUG_MSG(F("[update] Update ok."));
          Lynx::config.firmwareVersion = newFirmwareVersion;
          Lynx::config.save();
          Lynx::writeUDP(callerIP, callerPort, commands.prepareResponse("M,FU,1,").c_str());
          ESP.restart();
          break;
  }
}

