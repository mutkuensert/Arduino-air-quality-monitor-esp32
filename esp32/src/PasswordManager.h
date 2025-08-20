#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include "PreferenceConstants.h"
#include <Preferences.h>
#include <Arduino.h>
#include "Credentials.h"
#include "Preferences.h"
#include "Globals.h"

class PasswordManager
{
public:
    String getSsid();
    String getPassword();
    void setSsid(String ssid);
    void setPassword(String password);
};

extern PasswordManager passwordManager;

#endif