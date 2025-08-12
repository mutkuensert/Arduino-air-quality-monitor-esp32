#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <Arduino.h>
#include <WiFi.h>

class Connector {
public:
  HardwareSerial& serial;
  IPAddress localIp;
  IPAddress gateway;
  IPAddress subnet;

  Connector(HardwareSerial& serial, IPAddress localIp, IPAddress gateway, IPAddress subnet);

  bool connectToWifi(char* ssid, char* password);
  bool startAccessPoint(const char* ssid, const char* password);
};

#endif