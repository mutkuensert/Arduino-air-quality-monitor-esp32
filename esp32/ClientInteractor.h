#ifndef CLIENTINTERACTOR_H
#define CLIENTINTERACTOR_H
#include <WiFi.h>

class ClientInteractor {
public:
  WiFiClient& client;

  ClientInteractor(WiFiClient& client);

  void respondJson(char* json);
  void printHtml(char* html);
};

#endif