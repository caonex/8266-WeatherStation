#include "DisplayHandler.hpp"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

namespace DisplayHandler
{
    void displayReadings(float &fahrenheitTemp, float &humidity, float &heatIndex, float &temperatureOffset)
    {
        // Convert to celsius
        float celsius = (fahrenheitTemp - 32) * 5 / 9;

        // Clear the buffer.
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);

        display.setCursor(0, 0);
        // 21 characters wide
        display.println("Environment Readings"); //Environment Measurements
        //display.startscrollright(0x00, 0x00);

        clearLine(2);
        display.setCursor(0, 16);
        display.print(fahrenheitTemp);
        display.print((char)247);
        display.print("F");
        display.print("       ");
        display.print(String(celsius));
        display.print((char)247);
        display.println("C");
        display.println(" ");
        display.print(humidity);
        display.println("%RH");
        display.print("Celsius Offset: ");
        display.println(String(temperatureOffset));
        display.print("Heat Index: ");
        display.print(heatIndex);
        display.print((char)247);
        display.println("F");

        display.println(WiFi.localIP());

        display.display();
    }

    void clearLine(int ln)
    {
        display.setCursor(0, (ln - 1) * 8);

        // 21 Chars Wide
        display.print("                     ");
    }

    void clearBlock(int x, int y, int textBlocks)
    {
        String characters = "";
        display.setCursor(x, y);
        for (int x = 1; x <= textBlocks; x++)
        {
            characters += " ";
        }
    }

    bool begin()
    {
        return display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    }

    void setInitialState()
    {
        display.clearDisplay();

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(4, 16);
        // Display static text
        display.println("Loading...");
        display.display();
    }
} // namespace DisplayHandler