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
    String jsonString;

    std::unique_ptr<ESP8266WebServer> server;

    WebServer(short port, int &offsetAddress, float &temperatureOffset);
    void startMulticastDNSService(String name);
    void handleRoot();
    void handleNotFound();
    void defineEndPoints();
    void handleClient();
    void begin();

    template <class T>
    void getValueFromAddress(int address, T &t);

    template <typename T>
    void saveValueToAddress(int address, T &t);

    ~WebServer();
    // Offset variables
    float &temperatureOffset;

private:
    // Offset EEPROM (Flash) Address
    int offsetAddress;
};

template <class T>
void WebServer::getValueFromAddress(int address, T &t)
{
    EEPROM.get<T>(address, t);

    Serial.print("Read value ");
    Serial.print(t);
    Serial.print(" from ");
    Serial.println(address);

    return t;
}

template <typename T>
void WebServer::saveValueToAddress(int address, T &t)
{
    EEPROM.put<T>(address, t);
    EEPROM.commit();
    Serial.print("Saved value ");
    Serial.print(t);
    Serial.print(" to ");
    Serial.println(address);
}


#endif