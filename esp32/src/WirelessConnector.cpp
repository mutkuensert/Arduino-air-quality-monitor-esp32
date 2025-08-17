#include "WirelessConnector.h"

WirelessConnector::WirelessConnector(HardwareSerial& serial, IPAddress localIp, IPAddress gateway, IPAddress subnet)
  : serial(serial) {
  this->localIp = localIp;
  this->gateway = gateway;
  this->subnet = subnet;
}

bool WirelessConnector::connectToWifi(char* ssid, char* password) {
  serial.println();
  serial.print("Connecting to ");
  serial.println(ssid);

  WiFi.config(localIp, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter <= 10) {
    delay(500);
    serial.print(".");
    counter++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  serial.println("");
  serial.println("WiFi connected.");
  serial.println("IP address: ");
  serial.println(WiFi.localIP());
  serial.println("Signal strength (RSSI):");
  serial.println(WiFi.RSSI());
  return true;
}

bool WirelessConnector::startAccessPoint(const char* ssid, const char* password) {
  if (!WiFi.softAPConfig(localIp, gateway, subnet)) {
    serial.println("IP could not set!");
    return false;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  serial.print("Access Point has been started. IP: ");
  serial.println(WiFi.softAPIP());
  return true;
}