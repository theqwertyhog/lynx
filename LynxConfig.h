#ifndef LynxConfig_h
#define LynxConfig_h

#include <ArduinoJson.h>

class LynxConfig {
  public:
    LynxConfig();
    
    bool load();
    bool save();
    bool reset();

    char* deviceID;
    bool isDeviceEnabled;
    bool remoteAccessEnabled;
    char* devicePass;
    char* wifiMode;
    char* wifiSTASSID;
    char* wifiSTAPass;
    char* hostname;
    char* wifiAPPass;
    char* mqttServer;
    char* mqttUser;
    char* mqttPass; 
    char* commandsTopic;
    char* statusTopic;
    char* infoTopic;
    char* messagesTopic;
    char* firmwareVersion;

    String getConfigStr();
    
    private:
      char* buf;
      bool initFS();
};
#endif

