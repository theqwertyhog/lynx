#ifndef Lynx_h
#define Lynx_h

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.println( __VA_ARGS__ )
#else
#define DEBUG_MSG(...) 
#endif

#define APPLIANCE_STATE_OFF               0
#define APPLIANCE_STATE_ON                1

#include <WiFiUdp.h>

#include "LynxCommands.h"
#include "LynxConfig.h"
#include "LynxMQTT.h"
#include "LynxWifi.h"

class Lynx {
  public:
    Lynx();
    static const byte APPLIANCE_WRITE_PIN = 16;
    static const byte BTN_READ_PIN = 4; 

    static LynxCommands commands;
    static LynxConfig config;
    static LynxMQTT mqtt;
    static LynxWifi wifi;
    static WiFiUDP udp;

    static byte applianceState;
    static byte getApplianceState();
    static void setApplianceState(byte state);
    static byte getDefaultApplianceState();
    static void setDefaultApplianceState(byte state);
    static boolean isDeviceEnabled();

    static String getDeviceStatus();
    static String getDeviceInfo();

    void initmDNS(char* hostname);
    void udpLoop();
    static void writeUDP(IPAddress ip, uint16_t port, const char* data);
    static void updateFirmware(char* hostname, int port, char* newFirmwareVersion, IPAddress callerIP, uint16_t callerPort);

  private:
    void initEEPROM();
    void initPin();
    void initSerial();
    bool initWifi();
    bool initFS();
};

#endif

