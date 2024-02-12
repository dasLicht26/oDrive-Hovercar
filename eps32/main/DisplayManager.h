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


    void displayMessage(const String& message) {
      display.clearDisplay(); // Lösche den aktuellen Inhalt des Displays
      display.setTextSize(2); // Setze die Textgröße. 
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.setCursor(0, 0); // Setze den Cursor an den Anfang des Displays
      display.println(message); 
      display.display(); // Zeige die Änderungen auf dem Display an
    }

    void displaySpeedmode(String mode){
      display.drawRoundRect (0, 0, 16, 20,3, SSD1306_WHITE);
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.setCursor(3, 3); // Setze den Cursor an den Anfang des Displays
      display.setTextSize(2); // Setze die Textgröße. 
      display.println(mode); 
    }

    void displaySpeed(float requestKmh, float currentKmh, int maxKMH){
      if (requestKmh < 0){
        requestKmh = requestKmh * -1;
      }
      if (currentKmh < 0){
        currentKmh = currentKmh * -1;
      }

      //set kmh Text
      display.setCursor(18, 0); // Setze den Cursor hinter 'mode'
      display.setTextSize(1); 
      display.print("Kmh "); 
      //set kmh Value
      display.setTextSize(4); 
      int kmhInt = currentKmh + 0.5;
      String displayKmhValue;
      if (kmhInt < 10){
        displayKmhValue = "0" + String(kmhInt);
        }
      else {
        displayKmhValue = String(kmhInt);
        }
      display.print(displayKmhValue); 
      //set kmh bar
      display.drawRect (94, 0, 6, SCREEN_HEIGHT, SSD1306_WHITE);
      int rectHightCurrent = map(kmhInt, 0, maxKMH, 0, SCREEN_HEIGHT);
      display.fillRect(94, SCREEN_HEIGHT - rectHightCurrent, 6, rectHightCurrent, SSD1306_WHITE);
      int kmhIntCurrent = requestKmh +0.5;
      int rectHightRequest = map(kmhIntCurrent, 0, maxKMH, 0, SCREEN_HEIGHT);
      display.drawTriangle(100, SCREEN_HEIGHT - rectHightRequest, 103, SCREEN_HEIGHT - rectHightRequest +3, 103, SCREEN_HEIGHT - rectHightRequest-3, SSD1306_WHITE);
      display.drawTriangle(93, SCREEN_HEIGHT - rectHightRequest, 90, SCREEN_HEIGHT - rectHightRequest +3, 90, SCREEN_HEIGHT - rectHightRequest-3, SSD1306_WHITE);
    }

    void displayBat(int batPercent){
      display.fillRect(115, 0, 6, 2, SSD1306_WHITE);
      display.drawRoundRect (108, 2, 20, 30,3, SSD1306_WHITE);

      if (batPercent > 80){
        display.fillRect(110, 6, 16, 5, SSD1306_WHITE);
      }
      if (batPercent > 60){
        display.fillRect(110, 12, 16, 5, SSD1306_WHITE);
      }
      if (batPercent > 40){
        display.fillRect(110, 18, 16, 5, SSD1306_WHITE);
      }
      if (batPercent > 20){
        display.fillRect(110, 24, 16, 5, SSD1306_WHITE);
      }
    }

    void vBatLowError(){
      bool show = false;
      while(true){
        display.clearDisplay(); // Lösche Displayinhalt
        display.setCursor(0, 0); // Setze den Cursor hinter 'mode'
        display.setTextSize(2); 
        display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
        display.println("Batterie");
        display.print("leer");
        if (show == false){
          displayBat(21);
          show = true;
          }
        else{
          show=false;
          }
        display.display();
        delay(1000);
      }
    }

    void displayDashboard(float requestKmh, float currentKmh, int batPercent, String mode, int maxKMH){
      display.clearDisplay(); // Lösche Displayinhalt
      // bauer Displayinhalt neu auf
      displaySpeedmode(mode); // Setze Geschwindigkeitsmodus (R/1/2/3/4)
      displaySpeed(requestKmh, currentKmh, maxKMH); // Setze Kmh 
      displayBat(batPercent);
      
      display.display(); // Zeige die Änderungen auf dem Display an
    }


    void displayMessage(float number) {
      String message = String(number); // Konvertiere float zu String
      displayMessage(message); // Rufe die ursprüngliche Funktion au
    }

  private:
};

#endif