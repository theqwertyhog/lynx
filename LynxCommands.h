#ifndef LynxCommands_h
#define LynxCommands_h

#include <WebSocketsClient.h>

class LynxCommands {
  public:
    char* deviceHandshake = "Lynx/sock/v1";
    void handleCommand(char* commandStr, bool isRemote = false, IPAddress ip = IPAddress(), uint16_t port = 0);
    String prepareResponse(String data);
};
#endif

