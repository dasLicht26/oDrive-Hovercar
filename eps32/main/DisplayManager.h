#ifndef DisplayManager_h
#define DisplayManager_h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

// Definiere die GPIO-Pins für das I2C-Display
#define OLED_SDA 21 // GPIO21
#define OLED_SCL 22 // GPIO22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

class DisplayManager {
  public:
    void setup() {
      Wire.begin(OLED_SDA, OLED_SCL); // Initialisiere I2C mit SDA und SCL Pins
      if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
      }
      delay(2000);
      display.clearDisplay();
    }

    void displayValues(int value1, int value2) {
      display.clearDisplay();
      display.drawRect(10, 0, 20, SCREEN_HEIGHT, SSD1306_WHITE);
      display.fillRect(10, SCREEN_HEIGHT - map(value1, 0, 100, 0, SCREEN_HEIGHT), 20, SCREEN_HEIGHT, SSD1306_WHITE);
      display.drawRect(60, 0, 20, SCREEN_HEIGHT, SSD1306_WHITE);
      display.fillRect(60, SCREEN_HEIGHT - map(value2, 0, 100, 0, SCREEN_HEIGHT), 20, SCREEN_HEIGHT, SSD1306_WHITE);
      display.display();
    }
};

#endif