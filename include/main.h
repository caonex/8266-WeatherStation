// Configurations for the Temp & Humidity Sensor Library
#define DHTTYPE DHT11
// The pin must start with *D*, the physical pin 4 actually is GPIO2
// You can refer to https://www.electronicwings.com/nodemcu/nodemcu-gpio-with-arduino-ide
// for more information. It took me a while to figure this out.
#define DHTPIN D4

/***********************************************************
 * The following functions were simply included here
 * to allow them to be used regardless of the declaration 
 * placement in the implementation.
 **********************************************************/

// Run the provided lambda function on each interval tick
void performTaskOnlyEveryMS(long interval, void (*task)());

// Use the sensor to read the temperature & humidity
bool readTempHumidity(
    void (*reportMeasurements)(unsigned long &myChannelNumber, char *myWriteAPIKey, float &temperature, float &humidity, float &heatIndex),
    void (*displayReadings)(float &temperature, float &humidity, float &heatIndex, float &temperatureOffset));