#include <WiFi.h>
#include "time.h"
#include <Arduino.h>
#include "Credentials.h"
#include "WirelessConnector.h"
#include <WebServer.h> //https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino
#include "Sds011Reader.h"
#include <stdexcept>
#include "IpConfig.h"
#include <Preferences.h>
#include <nvs_flash.h>

constexpr char *NTP_SERVER = "pool.ntp.org";
constexpr long GMT_OFFSET_SEC = 3 * 3600;
constexpr int DAYLIGHT_OFFSET_SEC = 0;
constexpr int SIGNAL_PIN = 15;
constexpr char *PREFS_ESP32 = "ESP32";
constexpr char *KEY_PREFS_SSID = "SSID";
constexpr char *KEY_PREFS_PASSWORD = "PASSWORD";

WebServer server(80);
WirelessConnector wirelessConnector(Serial, LOCAL_IP, GATEWAY, SUBNET);
Sds011Reader sds011Reader(Serial);
Preferences preferences;

volatile bool isDataReady = false;

void IRAM_ATTR setDataReady()
{
  isDataReady = true;
}

void handleRoot()
{
  preferences.begin(PREFS_ESP32);
  String savedSsid = preferences.getString(KEY_PREFS_SSID, "");
  String savedPassword = preferences.getString(KEY_PREFS_PASSWORD, "");
  preferences.end();

  String formHtml = R"(
    <form action="/savePassword" method="GET">
      <label for="ssid">SSID:</label><br>
      <input type="text" id="ssid" name="ssid" value=")" +
                    savedSsid + R"("><br>
      <label for="password">Password:</label><br>
      <input type="text" id="password" name="password" value=")" +
                    savedPassword + R"("><br><br>
    <input type="submit" value="Save">
    </form>
  )";

  String htmlData = formHtml;
  String jsonData = "";

  if (isDataReady)
  {
    SensorData sensorData = sds011Reader.getLastSensorData();

    htmlData = formHtml + "<br>" + String("Wifi strength: " + String(WiFi.RSSI()) + "<br>" + "PM2.5: " + String(sensorData.pm25) + "<br>" + "PM10: " + String(sensorData.pm10));

    jsonData = String("{\"pm2.5\":") + String(sensorData.pm25) + "," + String("\"pm10\":") + String(sensorData.pm10) + String("}");

    Serial.println("Received data:");
    Serial.println(sensorData.pm25);
    Serial.println(sensorData.pm10);
  }

  server.send(200, "text/html", htmlData);
}

void handleSavePassword()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  preferences.begin(PREFS_ESP32);
  preferences.putString(KEY_PREFS_SSID, ssid);
  preferences.putString(KEY_PREFS_PASSWORD, password);
  preferences.end();

  String response = "Successfully saved.";
  server.send(200, "text/html", response);

  Serial.println("Received:");
  Serial.println("SSID: " + ssid);
  Serial.println("Password: " + password);
}

void cleanRom()
{
  nvs_flash_erase();
  nvs_flash_init();
}

void setup()
{
  Serial.begin(115200);

  preferences.begin(PREFS_ESP32);
  String savedSsid = preferences.getString(KEY_PREFS_SSID, "");
  String savedPassword = preferences.getString(KEY_PREFS_PASSWORD, "");
  preferences.end();

  if (savedSsid == "")
  {
    savedSsid = SSID;
    savedPassword = PASSWORD;
  }

  try
  {
    pinMode(SIGNAL_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), setDataReady, RISING);

    char savedSsidBuffer[50];
    char savedPasswordBuffer[50];
    savedSsid.toCharArray(savedSsidBuffer, 50);
    savedPassword.toCharArray(savedPasswordBuffer, 50);

    if (!wirelessConnector.connectToWifi(savedSsidBuffer, savedPasswordBuffer))
    {
      Serial.println("Access point is being started.");
      wirelessConnector.startAccessPoint("AccessPoint", "123456789");
    }

    server.on("/", handleRoot);
    // server.on("/json", handleJson);
    server.on("/savePassword", handleSavePassword);
    server.begin();
  }
  catch (const std::exception &e)
  {
    Serial.println(String("Error: ") + e.what());
  }
  catch (...)
  {
    Serial.println("An unknown error occured!");
  }
}

void loop()
{
  server.handleClient();
  delay(2);
}