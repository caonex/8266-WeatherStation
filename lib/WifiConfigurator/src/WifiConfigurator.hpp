#ifndef __WIFICONFIGURATOR_H__
#define __WIFICONFIGURATOR_H__

// All libraries for managing Wifi and Multi Configurations
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

namespace WifiConfigurator
{
    void setup(WiFiMode_t wifiMode, char secretSSID[], char secretPass[], char secretSSIDWork[], char secretPassWork[]);
    void connect();
} // namespace WifiConfigurator

#endif // __WIFICONFIGURATOR_H__