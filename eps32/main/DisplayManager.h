#ifndef DisplayManager_h
#define DisplayManager_h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Constants.h"
#include "Bootlogo.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
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

            // Dein Bitmap anzeigen
      display.drawBitmap( 0, 0, hoverLogo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

      display.display();
    }

    void displayLogo(){
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
      display.drawRoundRect (0, 0, 22, 30,5, SSD1306_WHITE);
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.setCursor(3, 5); // Setze den Cursor an den Anfang des Displays
      display.setTextSize(3); // Setze die Textgröße. 
      display.println(mode); 
    }

    void displayNm(float Nm){
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.setCursor(24, 0); // Setze den Cursor an den Anfang des Displays
      display.setTextSize(1); // Setze die Textgröße. 
      display.println(String(Nm) + "Nm"); 
    }

    void displaySpeed(float requestKmh, float currentKmh, int maxKMH){
      if (requestKmh < 0){
        requestKmh = requestKmh * -1;
      }
      if (currentKmh < 0){
        currentKmh = currentKmh * -1;
      }

      //set kmh Text
      display.setCursor(24, 18); // Setze den Cursor hinter 'mode'
      //set kmh Value
      display.setTextSize(7); 
      int kmhInt = currentKmh + 0.5;
      String displayKmhValue;
      if (kmhInt < 10){
        displayKmhValue = "0" + String(kmhInt);
        }
      else {
        displayKmhValue = String(kmhInt);
        }
      display.print(displayKmhValue); 
    }

    void displayBat(int batPercent, float vBat){
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
      display.setCursor(108, 34); // Setze den Cursor hinter 'mode'
      display.setTextSize(1); 
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.println(String(int(vBat + 0.5)) + "V");
    }

    void vBatLowError(float vBat){
      bool show = false;
      while(true){
        display.clearDisplay(); // Lösche Displayinhalt
        display.setCursor(0, 0); // Setze den Cursor hinter 'mode'
        display.setTextSize(2); 
        display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
        display.println("Batterie");
        display.print("leer");
        if (show == false){
          displayBat(21, vBat);
          show = true;
          }
        else{
          show=false;
          }
        display.display();
        delay(1000);
      }
    }

    void displayDashboard(float requestKmh, float currentKmh, int batPercent, float vBat, String mode, int maxKMH, float Nm){
      display.clearDisplay(); // Lösche Displayinhalt
      // bauer Displayinhalt neu auf
      displaySpeedmode(mode); // Setze Geschwindigkeitsmodus (R/1/2/3/4)
      displaySpeed(requestKmh, currentKmh, maxKMH); // Setze Kmh 
      displayBat(batPercent, vBat);
      displayNm(Nm);
      display.display(); // Zeige die Änderungen auf dem Display an
    }


    void displayMessage(float number) {
      String message = String(number); // Konvertiere float zu String
      displayMessage(message); // Rufe die ursprüngliche Funktion au
    }

  private:
};

#endif