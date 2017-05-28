#include "stdio.h"

#include "FS.h"
#include "LynxConfig.h"
#include "Lynx.h"
#include "stdlib_noniso.h"

LynxConfig::LynxConfig() {
  if(!initFS()) {
    DEBUG_MSG(F("FS mount failed"));
  }
  DEBUG_MSG(F("FS Mounted"));
  
  if(load()) {
    DEBUG_MSG(F("Config loaded"));
  }
}

bool LynxConfig::initFS() {
  DEBUG_MSG(F("Mounting FS"));
  return SPIFFS.begin();
}

bool LynxConfig::load() {
  DEBUG_MSG(F("Loading config..."));
  File configFile = SPIFFS.open(F("/config.conf"), "r");
  if (!configFile) {
    Serial.println(F("Failed to open config file"));
    return false;
  }

  size_t size = configFile.size();
  buf = (char*) malloc(size + 1);
  
  configFile.readBytes(buf, size);
  configFile.close();
  buf[size] = '\0';

  DEBUG_MSG(buf);

  deviceID = strtok(buf, ",");
  isDeviceEnabled = strcmp(strtok(NULL, ","), "1") == 0 ? true : false;
  remoteAccessEnabled = strcmp(strtok(NULL, ","), "1") == 0 ? true : false;
  devicePass = strtok(NULL, ",");
  wifiMode = strtok(NULL, ",");
  wifiSTASSID = strtok(NULL, ",");
  wifiSTAPass = strtok(NULL, ",");
  hostname = strtok(NULL, ",");
  wifiAPPass = strtok(NULL, ",");
  mqttServer = strtok(NULL, ",");
  mqttUser = strtok(NULL, ",");
  mqttPass = strtok(NULL, ",");
  commandsTopic = strtok(NULL, ",");
  statusTopic = strtok(NULL, ",");
  infoTopic = strtok(NULL, ",");
  messagesTopic = strtok(NULL, ",");
  firmwareVersion = strtok(NULL, ",");

  return true;
}

String LynxConfig::getConfigStr() {
  String info = deviceID;

  info.concat(',');
  info.concat(isDeviceEnabled);
  info.concat(',');
  info.concat(remoteAccessEnabled);
  info.concat(',');
  info.concat(devicePass);
  info.concat(',');
  info.concat(wifiMode);
  info.concat(',');
  info.concat(wifiSTASSID);
  info.concat(',');
  info.concat(wifiSTAPass);
  info.concat(',');
  info.concat(hostname);
  info.concat(',');
  info.concat(wifiAPPass);
  info.concat(',');
  info.concat(mqttServer);
  info.concat(',');
  info.concat(mqttUser);
  info.concat(',');
  info.concat(mqttPass);
  info.concat(',');
  info.concat(commandsTopic);
  info.concat(',');
  info.concat(statusTopic);
  info.concat(',');
  info.concat(infoTopic);
  info.concat(',');
  info.concat(messagesTopic);
  info.concat(',');
  info.concat(firmwareVersion);
  info.concat(',');

  return info;
}

bool LynxConfig::save() {
  DEBUG_MSG(F("Saving config..."));
  File configFile = SPIFFS.open(F("/config.conf"), "w");
  if (!configFile) {
    DEBUG_MSG(F("Failed to open config file for writing"));
    return false;
  }

  DEBUG_MSG(getConfigStr());
  
  configFile.print(getConfigStr());
  configFile.close();
  
  return true;
}

bool LynxConfig::reset() {
  File factoryConfigFile = SPIFFS.open(F("/factory_config.conf"), "r");
  File configFile = SPIFFS.open(F("/config.conf"), "w");

  size_t size = factoryConfigFile.size();
  char buf[size];
  
  factoryConfigFile.readBytes(buf, size);
  configFile.print(buf);

  configFile.close();
  factoryConfigFile.close();

  Lynx::setDefaultApplianceState(1);

  ESP.restart();
  
  return true;
}
