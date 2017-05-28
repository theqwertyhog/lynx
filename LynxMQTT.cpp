#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "LynxConfig.h"
#include "LynxCommands.h"
#include "LynxMQTT.h"
#include "Lynx.h"

LynxMQTT::LynxMQTT() : client(wifiClient) { 
  client.setServer(Lynx::config.mqttServer, 28045);
  client.setCallback(callback);
}

void LynxMQTT::connect() {
  if (!Lynx::config.remoteAccessEnabled) {
    if (client.connected()) {
      client.disconnect();
    }
    return;
  }

  if (client.connected()) {
    return;
  }

    DEBUG_MSG(F("Attempting MQTT connection..."));

    if (client.connect(Lynx::config.deviceID, Lynx::config.mqttUser, Lynx::config.mqttPass)) {
      DEBUG_MSG(F("MQTT connected"));
      DEBUG_MSG(Lynx::config.commandsTopic);
      client.subscribe(Lynx::config.commandsTopic);
    } else {
     DEBUG_MSG(client.state());
     DEBUG_MSG(F("Retrying MQTT..."));
    }
}

void LynxMQTT::callback(char* topic, byte* payload, unsigned int length) {
  DEBUG_MSG((char*) payload);
  Lynx::commands.handleCommand((char*) payload, true);
}

