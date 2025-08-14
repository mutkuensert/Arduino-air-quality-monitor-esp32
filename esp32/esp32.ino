#include <WiFi.h>
#include "time.h"
#include <Arduino.h>
#include "Credentials.h"
#include "Config.h"
#include "WirelessConnector.h"
#include "ClientInteractor.h"

constexpr char* NTP_SERVER = "pool.ntp.org";
constexpr long GMT_OFFSET_SEC = 3 * 3600;
constexpr int DAYLIGHT_OFFSET_SEC = 0;
constexpr int SIGNAL_PIN = 15;
WiFiServer server(80);
WirelessConnector wirelessConnector(Serial, localIp, gateway, subnet);
long rssi = 0;

volatile bool isDataReady = false;
constexpr uint8_t DATA_HEAD = 0xAA;

char htmlData[200];  //It's a char array just for an example
char jsonData[100];

void IRAM_ATTR setDataReady() {
  isDataReady = true;
}

struct tm getLocalTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Time info could not be retrieved.");
    // Set all fields to zero to return invalid time
    memset(&timeinfo, 0, sizeof(struct tm));
  }

  return timeinfo;
}

void setup() {
  pinMode(SIGNAL_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), setDataReady, RISING);

  Serial.begin(115200);

  delay(10);

  if (!wirelessConnector.connectToWifi(ssid, password)) {
    wirelessConnector.startAccessPoint("AccessPoint", "12345");
  }

  //configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  server.begin();
}

void waitForData() {
  while (Serial.available() < 4) {
    delay(1);
  }
}

void readUntilDataHead() {
  while (Serial.read() != DATA_HEAD) {
    delay(1);
  }
}

int convertHighLowByteToDecimal(uint8_t high, uint8_t low) {
  return (high << 8) | low;
}

float* getSensorData() {
  isDataReady = false;
  static float data[2];
  String currentLine = "";
  int lineIndex = 0;

  readUntilDataHead();
  int pm25Low = Serial.read();
  int pm25High = Serial.read();
  int pm10Low = Serial.read();
  int pm10High = Serial.read();
  float pm25 = convertHighLowByteToDecimal(pm25High, pm25Low) / 10.0;
  float pm10 = convertHighLowByteToDecimal(pm10High, pm10Low) / 10.0;

  data[0] = pm25;
  data[1] = pm10;

  return data;
}

void loop() {
  if (isDataReady) {
    waitForData();
    float* data = getSensorData();

    snprintf(htmlData, sizeof(htmlData),
             "Wifi strength: %d<br>"
             "PM2.5: %.1f<br>"
             "PM10: %.1f",
             rssi, data[0], data[1]);

    snprintf(jsonData, sizeof(jsonData),
             "{"
             "\"pm2.5\":%.1f,"
             "\"pm10\":%.1f"
             "}",
             data[0], data[1]);

    Serial.println("Received data:");
    Serial.println(data[0]);
    Serial.println(data[1]);
  }

  WiFiClient client = server.available();
  ClientInteractor clientInteractor = ClientInteractor(client);

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    bool isEndOfRequest = false;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (c != '\r') {
          currentLine += c;
        }

        if (currentLine == "\n") {
          isEndOfRequest = true;
        } else if (c == '\n' && currentLine.length() != 0) {
          currentLine = "";
        }

        Serial.write(c);

        if (currentLine.startsWith("GET /json")) {
          clientInteractor.respondJson(jsonData);
          break;
        }

        if (isEndOfRequest) {
          clientInteractor.printHtml(htmlData);
          break;
        }
      }
    }

    delay(1);  // give the web browser time to receive the data
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
