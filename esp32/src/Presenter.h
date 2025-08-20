#ifndef PRESENTER_H
#define PRESENTER_H

#include <Arduino.h>
#include "PasswordManager.h"
#include "Log.h"
#include "Sds011Reader.h"
#include <WiFi.h>
#include <WebServer.h> //https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino

class Presenter
{
public:
    void startServer();
    void handleClient();
    static void IRAM_ATTR setDataReady();

private:
    void handleRoot();
    void handleJson();
    void handleSavePassword();
};

#endif