#include <WebSocketsClient.h>

#include "Lynx.h"
#include "LynxCommands.h"

void LynxCommands::handleCommand(char* commandStr, bool isRemote, IPAddress ip, uint16_t port) {
  char* handshake = strtok(commandStr, ",");
  if (strcmp(handshake, deviceHandshake) != 0) {
    return;
  }
  char command = strtok(NULL, ",")[0];

  switch (command) {
    case 'a':
      DEBUG_MSG(F("[Command] Updating appliance state"));
      Lynx::setApplianceState(atoi(strtok(NULL, ",")));
      break;

    case 'b':
      DEBUG_MSG(F("[Command] Updating remote access"));
      Lynx::config.remoteAccessEnabled = atoi(strtok(NULL, ","));
      Lynx::config.save();
      break;

    case 'c': {
      DEBUG_MSG(F("Sending status"));
      
      String response = prepareResponse(Lynx::getDeviceStatus());
      if (isRemote) {
        Lynx::mqtt.client.publish(Lynx::config.statusTopic, response.c_str());
      } else {
        Lynx::writeUDP(ip, port, response.c_str());
      }
      break;
    }

    case 'd': {
      DEBUG_MSG(F("Sending device info"));

      String response = prepareResponse(Lynx::getDeviceInfo());
      if (isRemote) {
        Lynx::mqtt.client.publish(Lynx::config.infoTopic, response.c_str());
      } else {
        Lynx::writeUDP(ip, port, response.c_str());
      }
      break;
    }

    case 'e':
      if (isRemote) { return; }
      DEBUG_MSG(F("Connecting to new STA"));
      Lynx::config.wifiSTASSID = strtok(NULL, ",");
      Lynx::config.wifiSTAPass = strtok(NULL, ",");
      Lynx::config.save();
      Lynx::wifi.stopSTA();
      Lynx::wifi.startSTA();
      break;

    case 'f':
      DEBUG_MSG(F("Blocking device"));
      Lynx::wifi.stopSTA();
      Lynx::wifi.stopAP();
      Lynx::config.isDeviceEnabled = false;
      Lynx::config.save();
      break;

    case 'g':
      if (isRemote) { return; }
      DEBUG_MSG(F("Updating firmware"));
      Lynx::updateFirmware(strtok(NULL, ","), atoi(strtok(NULL, ",")), strtok(NULL, ","), ip, port);
      break;

    case 'h':
      if (isRemote) { return; }
      DEBUG_MSG(F("Setting default appliance state"));
      Lynx::setDefaultApplianceState(atoi(strtok(NULL, ",")));
      break;
  }
}

String LynxCommands::prepareResponse(String data) {
  String response = deviceHandshake;
  response.concat(',');
  response.concat(data);

  return response;
}

