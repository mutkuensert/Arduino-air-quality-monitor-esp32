#include "Presenter.h"

Sds011Reader sds011Reader(Serial);
WebServer server(80);
volatile bool isDataReady = false;

void IRAM_ATTR Presenter::setDataReady()
{
    isDataReady = true;
}

void Presenter::handleRoot()
{
    String savedSsid = passwordManager.getSsid();
    String savedPassword = passwordManager.getPassword();

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

        htmlData = formHtml + "<br>" + "Wifi strength: " + String(WiFi.RSSI()) + "<br>" + "PM2.5: " + String(sensorData.pm25) + "<br>" + "PM10: " + String(sensorData.pm10);

        Logfln("Received data: Pm2.5: %f, Pm10: %f", sensorData.pm25, sensorData.pm10);
    }
    else
    {
        htmlData = formHtml + "<br>" + "Data is not ready.";
    }

    server.send(200, "text/html", htmlData);
}

void Presenter::handleJson()
{
    String jsonData = "";

    if (isDataReady)
    {
        SensorData sensorData = sds011Reader.getLastSensorData();
        jsonData = String("{\"pm2.5\":") + String(sensorData.pm25) + "," + String("\"pm10\":") + String(sensorData.pm10) + String("}");

        Logfln("Received data: Pm2.5: %f, Pm10: %f", sensorData.pm25, sensorData.pm10);
    }

    server.send(200, "application/json", jsonData);
}

void Presenter::handleSavePassword()
{
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    passwordManager.setSsid(ssid);
    passwordManager.setSsid(password);

    String response = "Successfully saved.";
    server.send(200, "text/html", response);

    Logfln("Received SSID: %s, Password: %s", ssid, password);
}

void Presenter::startServer()
{
    server.on("/", [this]()
              { handleRoot(); });
    server.on("/json", [this]()
              { handleJson(); });
    server.on("/savePassword", [this]()
              { handleSavePassword(); });
    server.begin();
}

void Presenter::handleClient()
{
    server.handleClient();
}