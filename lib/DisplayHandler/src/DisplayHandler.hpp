#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Just to have access to the Wifi constant, the include guard will actually
// prevent it from binding or loading twice.
#include "WifiConfigurator.hpp"

// Define the screen width and height
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

namespace DisplayHandler
{
    // Function used to display the readings on a OLED display
    void displayReadings(float &fahrenheitTemp, float &humidity, float &heatIndex, float &temperatureOffset);

    // Used to clear the entire line, it reprints full empty spaces on pixels
    void clearLine(int ln);

    // Clears a single block starting with coords x and y.
    void clearBlock(int x, int y, int textBlocks);

    bool begin();

    void setInitialState();
} // namespace DisplayHandler
