#ifndef WIRELESSCONNECTOR_H
#define WIRELESSCONNECTOR_H

#include <Arduino.h>
#include <WiFi.h>

class WirelessConnector {
public:
  IPAddress localIp;
  IPAddress gateway;
  IPAddress subnet;

  WirelessConnector(IPAddress localIp, IPAddress gateway, IPAddress subnet);

  bool connectToWifi(char* ssid, char* password);
  bool startAccessPoint(const char* ssid, const char* password);
};

#endif