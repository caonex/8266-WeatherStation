#include <Arduino.h>
#include "main.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <secrets.hpp>
#include <WifiConfigurator.hpp>
#include <WebServer.hpp>
#include <TaskScheduler.h>

#include "DisplayHandler.hpp"

#include "ThingSpeakProcessor.hpp"

// Used for calibrating the temperature to a benchmark or another device.
float temperatureOffset = 0.00;

// The flash memory address where the temperature offset will be stored and retrieved from.
int offsetAddress = 0;

// ThingSpeak information from the secrets.hpp file.
unsigned long thingSpeakChannelNumber = SECRET_CH_ID;
char *thingSpeakWriteKey = SECRET_WRITE_APIKEY;

// Instance of DHT which is used to read from either DHT11 or DHT22 temperature & humidity sensors
DHT dht(DHTPIN, DHTTYPE); // Declaration for DHT11, the default count = 6 will be the default for third parameter.
//DHT dht(DHTPIN, DHTTYPE, 30);   // Declaration for DHT22, without the 30 counts, it does not work.

// WIFI Settings
char ssid[] = PRIMARY_SECRET_SSID; // your network SSID (name)
char pass[] = PRIMARY_SECRET_PASS; // your network password
char ssidWork[] = SECONDARY_SECRET_SSID;
char passWork[] = SECONDARY_SECRET_PASS;
int keyIndex = 0; // your network key Index number (needed only for WEP)

// New WIFI Client
WiFiClient client;

// Create an instance of WebServer in the stack, it is a project only library.
// It is defined in the lib folder.
// This library is also managing the access to the simulated EEPROM as it gets input from user
// and stores it.
WebServer ws(80, offsetAddress, temperatureOffset);

void setup()
{
    // Initialize serial
    Serial.begin(9600);

    // Read Offset from Flash
    ws.getValueFromAddress<float>(offsetAddress, temperatureOffset);

    // Initialize device
    dht.begin();

    WifiConfigurator::setup(WIFI_STA, ssid, pass, ssidWork, passWork);

    // Initialize ThingSpeak
    ThingSpeakProcessor::beginWithClient(client);

    // Start the web server
    ws.begin();

    // Configure Built-in LED pin as output to use it, yay!!
    pinMode(LED_BUILTIN, OUTPUT);

    // Start mDNS Server for broadcasting weather.local for devices to connect without knowing ip
    ws.startMulticastDNSService("weatherstation");

    // Attempt to initialize the display.
    if (!DisplayHandler::begin())
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
    }
}

void loop()
{
    // Respond to MDNS requests
    MDNS.update();

    // Ensure device is connected to Wifi, otherwise attempt to cycle through known SSIDs
    WifiConfigurator::connect();

    // Read Temp & Humidity every 60000
    performTaskOnlyEveryMS(60000, []() {
        readTempHumidity(&ThingSpeakProcessor::reportMeasurement, &DisplayHandler::displayReadings);
    });

    // Listen and handle any http client hitting our built-in site.
    ws.handleClient();
}

void performTaskOnlyEveryMS(long interval, void (*task)())
{
    static const unsigned long REFRESH_INTERVAL = interval; // ms
    static unsigned long lastRefreshTime = 0;

    // Perform task only if applicable
    if (millis() - lastRefreshTime >= REFRESH_INTERVAL)
    {
        lastRefreshTime += REFRESH_INTERVAL;

        // Perform the desired task
        (*task)();
    }
}

bool readTempHumidity(
    void (*reportMeasurements)(unsigned long &myChannelNumber, char *myWriteAPIKey, float &temperature, float &humidity, float &heatIndex),
    void (*displayReadings)(float &temperature, float &humidity, float &heatIndex, float &temperatureOffset))
{
    // Read Temperature, humidity, and compute heat index
    float temperature = (dht.readTemperature() + temperatureOffset) * 9 / 5 + 32;
    float humidity = dht.readHumidity();
    float heatIndex = dht.computeHeatIndex(temperature, humidity);

    // If any is not a number, skip reporting
    if (isnan(temperature) || isnan(humidity) || isnan(heatIndex))
    {
        Serial.println(F("Error reading temperature & humidity!"));
        return false;
    }

    if (Serial)
    {
        Serial.print(F("Temperature: "));
        Serial.print(temperature);
        Serial.println(F("°F"));

        Serial.print(F("Humidity: "));
        Serial.print(humidity);
        Serial.println(F("%"));

        Serial.print(F("Heat Index: "));
        Serial.println(heatIndex);
    }

    // Report measurements
    (*reportMeasurements)(thingSpeakChannelNumber, thingSpeakWriteKey, temperature, humidity, heatIndex);

    // Display Readings on OLED Display
    (*displayReadings)(temperature, humidity, heatIndex, temperatureOffset);

    // Record the data for website to report it
    ws.doc["temperature"] = temperature;
    ws.doc["celsius"] = (temperature - 32) * 5 / 9;
    ws.doc["humidity"] = humidity;
    ws.doc["heatIndex"] = heatIndex;
    ws.doc["celsiusOffset"] = temperatureOffset;
    serializeJson(ws.doc, Serial);

    return true;
}
