#include "WirelessConnector.h"

WirelessConnector::WirelessConnector(IPAddress localIp, IPAddress gateway, IPAddress subnet)
{
  this->localIp = localIp;
  this->gateway = gateway;
  this->subnet = subnet;
}

bool WirelessConnector::connectToWifi(String ssid, String password)
{
  Logfln("Connecting to %s", ssid);

  WiFi.config(localIp, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int counter = 0;
  LogPlainLn("");
  while (WiFi.status() != WL_CONNECTED && counter <= 10)
  {
    delay(500);
    LogPlain(".");
    counter++;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    return false;
  }

  Logln("");
  Logln("WiFi connected.");
  Logln("IP address: ");
  Logln(WiFi.localIP());
  Logln("Signal strength (RSSI):");
  Logln(WiFi.RSSI());
  return true;
}

bool WirelessConnector::startAccessPoint(const char *ssid, const char *password)
{
  if (!WiFi.softAPConfig(localIp, gateway, subnet))
  {
    Logln("IP could not set!");
    return false;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Log("Access Point has been started. IP: ");
  LogPlainLn(WiFi.softAPIP());
  return true;
}