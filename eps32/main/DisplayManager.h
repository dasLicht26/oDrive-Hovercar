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
    void setup(int _maxValue, int _minValue) {
      maxValue = _maxValue;
      minValue = _minValue;

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
      display.fillRect(10, SCREEN_HEIGHT - map(value1, minValue, maxValue, 0, SCREEN_HEIGHT), 20, SCREEN_HEIGHT, SSD1306_WHITE);
      display.drawRect(60, 0, 20, SCREEN_HEIGHT, SSD1306_WHITE);
      display.fillRect(60, SCREEN_HEIGHT - map(value2, minValue, maxValue, 0, SCREEN_HEIGHT), 20, SCREEN_HEIGHT, SSD1306_WHITE);
      display.display();
    }

    void displayMessage(const String& message) {
      display.clearDisplay(); // Lösche den aktuellen Inhalt des Displays
      display.setTextSize(1.5); // Setze die Textgröße. Du kannst diesen Wert anpassen.
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.setCursor(0, 0); // Setze den Cursor an den Anfang des Displays
      display.println(message); // Drucke die Fehlermeldung
      display.display(); // Zeige die Änderungen auf dem Display an
    }

    void displayMessage(float number) {
      String message = String(number); // Konvertiere float zu String
      displayMessage(message); // Rufe die ursprüngliche Funktion au
    }

  private:
    int maxValue;
    int minValue;
};

#endif