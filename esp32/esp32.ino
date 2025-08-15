#include <WiFi.h>
#include "time.h"
#include <Arduino.h>
#include "Credentials.h"
#include "Config.h"
#include "WirelessConnector.h"
#include <WebServer.h>
#include "Sds011Reader.h"

constexpr char* NTP_SERVER = "pool.ntp.org";
constexpr long GMT_OFFSET_SEC = 3 * 3600;
constexpr int DAYLIGHT_OFFSET_SEC = 0;
constexpr int SIGNAL_PIN = 15;
WebServer server(80);
WirelessConnector wirelessConnector(Serial, localIp, gateway, subnet);
Sds011Reader sds011Reader(Serial);
long rssi = 0;

volatile bool isDataReady = false;

void IRAM_ATTR setDataReady() {
  isDataReady = true;
}

void setup() {
  pinMode(SIGNAL_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), setDataReady, RISING);

  Serial.begin(115200);

  if (!wirelessConnector.connectToWifi(SSID, PASSWORD)) {
    wirelessConnector.startAccessPoint("AccessPoint", "12345");
  }

  server.on("/", handleRoot);
  //server.on("/json", handleJson);
  //server.on("/savePassword", savePassword);
  server.begin();
}


void loop() {
  server.handleClient();
}

void handleRoot() {
  String formHtml = R"rawliteral(
    <form action="/savePassword" method="GET">
      <label for="ssid">SSID:</label><br>
      <input type="text" id="ssid" name="ssid" value=""><br>
      <label for="password">Password:</label><br>
      <input type="text" id="password" name="password" value=""><br><br>
      <input type="save" value="Save">
    </form>
  )rawliteral";

  String htmlData = formHtml;
  String jsonData = "";

  if (isDataReady) {
    SensorData sensorData = sds011Reader.getSensorData();

    htmlData = formHtml + "<br>" + String("Wifi strength: " + String(rssi) + "<br>" + "PM2.5: " + String(sensorData.pm25) + "<br>" + "PM10: " + String(sensorData.pm10));

    jsonData = String("{\"pm2.5\":") + String(sensorData.pm25) + "," + String("\"pm10\":") + String(sensorData.pm10) + String("}");

    Serial.println("Received data:");
    Serial.println(sensorData.pm25);
    Serial.println(sensorData.pm10);
  }

  server.send(200, "text/html", htmlData);
}