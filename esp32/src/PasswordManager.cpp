#include "PasswordManager.h"

PasswordManager passwordManager;

String PasswordManager::getSsid()
{
    preferences.begin(PREFS_ESP32);
    String savedSsid = preferences.getString(KEY_PREFS_SSID, "");
    preferences.end();

    if (savedSsid == "")
    {
        savedSsid = SSID;
    }

    return savedSsid;
}

String PasswordManager::getPassword()
{
    preferences.begin(PREFS_ESP32);
    String savedPassword = preferences.getString(KEY_PREFS_PASSWORD, "");
    preferences.end();

    if (savedPassword == "")
    {
        savedPassword = PASSWORD;
    }

    return savedPassword;
}

void PasswordManager::setSsid(String ssid)
{
    preferences.begin(PREFS_ESP32);
    preferences.putString(KEY_PREFS_SSID, ssid);
    preferences.end();
}

void PasswordManager::setPassword(String password)
{
    preferences.begin(PREFS_ESP32);
    preferences.putString(KEY_PREFS_PASSWORD, password);
    preferences.end();
}