#include "time.h"
#include <Arduino.h>
#include "Credentials.h"
#include "WirelessConnector.h"
#include "Sds011Reader.h"
#include <stdexcept>
#include "IpConfig.h"
#include <nvs_flash.h>
#include "Log.h"
#include "Presenter.h"
#include "PasswordManager.h"

constexpr char *NTP_SERVER = "pool.ntp.org";
constexpr long GMT_OFFSET_SEC = 3 * 3600;
constexpr int DAYLIGHT_OFFSET_SEC = 0;
constexpr int SIGNAL_PIN = 15;

WirelessConnector wirelessConnector(LOCAL_IP, GATEWAY, SUBNET);
Presenter presenter;

void cleanRom()
{
  nvs_flash_erase();
  nvs_flash_init();
}

void setup()
{
  Serial.begin(115200);

  try
  {
    pinMode(SIGNAL_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), Presenter::setDataReady, RISING);

    if (!wirelessConnector.connectToWifi(passwordManager.getSsid(), passwordManager.getPassword()))
    {
      Logln("");
      Logln("Wifi connection is unsuccessful. Access point is being started.");
      wirelessConnector.startAccessPoint("AccessPoint", "123456789");
    }

    presenter.startServer();
  }
  catch (const std::exception &e)
  {
    Logln(String("Error: ") + e.what());
  }
  catch (...)
  {
    Logln("An unknown error occured!");
  }
}

void loop()
{
  presenter.handleClient();
  delay(2);
}