#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

namespace ThingSpeakProcessor
{
    void beginWithClient(WiFiClient &client);
    void reportMeasurement(unsigned long &myChannelNumber, char *myWriteAPIKey, float &temperature, float &humidity, float &heatIndex);
} // namespace ThingSpeakProcessor