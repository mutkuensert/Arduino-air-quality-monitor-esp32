#include "ClientInteractor.h"

ClientInteractor::ClientInteractor(WiFiClient& client)
  : client(client){}

void ClientInteractor::respondJson(char* json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json; charset=UTF-8");
  client.println();
  client.println(json);
  client.println();
}

void ClientInteractor::printHtml(char* html) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html; charset=UTF-8");
  client.println();
  client.println(html);
  client.println();
}