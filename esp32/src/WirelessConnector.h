#ifndef WIRELESSCONNECTOR_H
#define WIRELESSCONNECTOR_H

#include <Arduino.h>
#include <WiFi.h>
#include "Log.h"

class WirelessConnector {
public:
  IPAddress localIp;
  IPAddress gateway;
  IPAddress subnet;

  WirelessConnector(IPAddress localIp, IPAddress gateway, IPAddress subnet);

  bool connectToWifi(String ssid, String password);
  bool startAccessPoint(const char* ssid, const char* password);
};

#endif