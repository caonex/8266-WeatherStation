#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

class WebServer
{
public:
    const size_t CAPACITY;
    DynamicJsonDocument doc;
    JsonObject data;

    ESP8266WebServer server;

    WebServer(short port, int &offsetAddress, float &temperatureOffset);
    void startMulticastDNSService(String name);
    void handleRoot();
    void handleNotFound();
    void defineEndPoints();
    void handleClient();
    void begin();

private:
    // Offset EEPROM (Flash) Address
    int offsetAddress;
    // Offset variables
    float temperatureOffset;
};

#endif