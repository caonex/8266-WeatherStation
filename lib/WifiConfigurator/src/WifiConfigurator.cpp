#include "WifiConfigurator.hpp"

ESP8266WiFiMulti wifiMulti;

namespace WifiConfigurator
{
    void setup(WiFiMode_t wifiMode, char secretSSID[], char secretPass[], char secretSSIDWork[], char secretPassWork[])
    {
        // Setup multiple WIFI Access points, HOME & WORK
        wifiMulti.addAP(secretSSID, secretPass);
        wifiMulti.addAP(secretSSIDWork, secretPassWork);

        // Set WIFI Mode
        WiFi.mode(wifiMode);
    }

    void connect()
    {
        // Connect or reconnect to WiFi
        if (wifiMulti.run() != WL_CONNECTED)
        {
            Serial.print("Attempting to connect to SSID: ");
            Serial.println(WiFi.SSID());
            while (wifiMulti.run() != WL_CONNECTED)
            {
                //WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
                Serial.print(".");
                delay(5000);
            }
            Serial.print("Connected to ");
            Serial.println(WiFi.SSID()); // Tell us what network we're connected to
            Serial.print("IP address:\t");
            Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
        }
    }
} // namespace WifiConfigurator