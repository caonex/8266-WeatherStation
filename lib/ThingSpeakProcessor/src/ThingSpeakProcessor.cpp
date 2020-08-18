#include "ThingSpeakProcessor.hpp"

void ThingSpeakProcessor::beginWithClient(WiFiClient &client)
{
    // Initialize ThingSpeak
    ThingSpeak.begin(client);
}

void ThingSpeakProcessor::reportMeasurement(unsigned long &myChannelNumber, char *myWriteAPIKey, float &temperature, float &humidity, float &heatIndex)
{
    // Set the fields with the values
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, heatIndex);
    ThingSpeak.setField(4, WiFi.localIP().toString());
    ThingSpeak.setField(5, WiFi.SSID());

    // Set the status
    //ThingSpeak.setStatus(myStatus);

    // Write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200)
    {
        Serial.println("Channel update successful.");
        digitalWrite(LED_BUILTIN, LOW);  // turn the inverted LED on (LOW is the voltage level)
        delay(1000);                     // wait for a second
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage HIGH
    }
    else
    {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
}
