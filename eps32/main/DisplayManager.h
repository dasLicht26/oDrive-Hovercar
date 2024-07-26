#ifndef DisplayManager_h
#define DisplayManager_h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Constants.h"
#include "Bootlogo.h"
#include "SpeedController.h"
#include "ConfigManager.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

class DisplayManager {
  public:
    void setup() {

      Wire.begin(OLED_SDA, OLED_SCL); // Initialisiere I2C mit SDA und SCL Pins
      Wire.setClock(50000); // Setzt die Taktrate auf 50 kHz (standard ist 100kHz) für stabielere Bilder
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
      display.setTextSize(1); // Setze die Textgröße. 
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


    void displayErrors(ODriveErrors* errors, int errorCount) {
        display.clearDisplay(); // Lösche den aktuellen Inhalt des Displays
        display.setTextSize(1); // Setze die Textgröße
        display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe

        for (int i = 0; i < errorCount; i++) {
            display.setCursor(0, i * 10); // Setze den Cursor an die entsprechende Position
            display.print("Source: ");
            display.print(errors[i].source);
            display.print(" Code: ");
            display.println(errors[i].errorCode);
        }

        display.display(); // Zeige die Änderungen auf dem Display an
    }


    void updateMenu(bool buttonOK, bool buttonUP, bool buttonDOWN, SpeedController& speedController, ConfigManager& configManager) {
        static bool lastButtonOK = false;
        static bool lastButtonUP = false;
        static bool lastButtonDOWN = false;

        if (buttonOK && !lastButtonOK) {
            switch (currentMenuState) {
                case MENU_MAIN:
                    currentMenuState = MENU_ADJUST_VEL_GAIN;
                    break;
                case MENU_ADJUST_VEL_GAIN:
                    currentMenuState = MENU_ADJUST_VEL_INTEGRATOR_GAIN;
                    break;
                case MENU_ADJUST_VEL_INTEGRATOR_GAIN:
                    currentMenuState = MENU_ADJUST_CONTROL_MODE;
                    break;
                case MENU_ADJUST_CONTROL_MODE:
                    currentMenuState = MENU_SAVE_SETTINGS;
                    break;
                case MENU_SAVE_SETTINGS:
                    saveSettings(speedController, configManager);
                    currentMenuState = MENU_MAIN;
                    break;
            }
        }

        if (buttonUP && !lastButtonUP) {
            switch (currentMenuState) {
                case MENU_ADJUST_VEL_GAIN:
                    speedController.setVelocityGain(speedController.getVelocityGain() + 0.1);
                    break;
                case MENU_ADJUST_VEL_INTEGRATOR_GAIN:
                    speedController.setVelocityIntegratorGain(speedController.getVelocityIntegratorGain() + 0.1);
                    break;
                case MENU_ADJUST_CONTROL_MODE:
                    if (speedController.getControlMode() == VELOCITY_CONTROL) {
                        speedController.setControlMode(TORQUE_CONTROL);
                    } else {
                        speedController.setControlMode(VELOCITY_CONTROL);
                    }
                    break;
            }
        }

        if (buttonDOWN && !lastButtonDOWN) {
            switch (currentMenuState) {
                case MENU_ADJUST_VEL_GAIN:
                    speedController.setVelocityGain(speedController.getVelocityGain() - 0.1);
                    break;
                case MENU_ADJUST_VEL_INTEGRATOR_GAIN:
                    speedController.setVelocityIntegratorGain(speedController.getVelocityIntegratorGain() - 0.1);
                    break;
                case MENU_ADJUST_CONTROL_MODE:
                    if (speedController.getControlMode() == VELOCITY_CONTROL) {
                        speedController.setControlMode(TORQUE_CONTROL);
                    } else {
                        speedController.setControlMode(VELOCITY_CONTROL);
                    }
                    break;
            }
        }

        lastButtonOK = buttonOK;
        lastButtonUP = buttonUP;
        lastButtonDOWN = buttonDOWN;

        displayMenu(speedController.getVelocityGain(), speedController.getVelocityIntegratorGain(), speedController.getControlMode());
    }

    void displayMenu(float vel_gain, float vel_integrator_gain, ControlMode control_mode) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);

        switch (currentMenuState) {
            case MENU_MAIN:
                display.println("Main Menu");
                display.println("1. Vel Gain");
                display.println("2. Vel Int Gain");
                display.println("3. Control Mode");
                display.println("4. Save Settings");
                break;
            case MENU_ADJUST_VEL_GAIN:
                display.println("Adjust Vel Gain");
                display.print("Value: ");
                display.println(vel_gain);
                display.println("UP/DOWN to change");
                display.println("OK to confirm");
                break;
            case MENU_ADJUST_VEL_INTEGRATOR_GAIN:
                display.println("Adjust Vel Int Gain");
                display.print("Value: ");
                display.println(vel_integrator_gain);
                display.println("UP/DOWN to change");
                display.println("OK to confirm");
                break;
            case MENU_ADJUST_CONTROL_MODE:
                display.println("Adjust Control Mode");
                display.print("Current: ");
                display.println(control_mode == VELOCITY_CONTROL ? "Velocity" : "Torque");
                display.println("UP/DOWN to toggle");
                display.println("OK to confirm");
                break;
            case MENU_SAVE_SETTINGS:
                display.println("Save Settings");
                display.println("Settings saved!");
                break;
        }

        display.display();
    }
  private:
    MenuState currentMenuState = MENU_MAIN;

    void saveSettings(SpeedController& speedController, ConfigManager& configManager) {
        Settings settings;
        settings.speedMode = speedController.getSpeedMode();
        settings.controlMode = speedController.getControlMode();
        settings.velocityGain = speedController.getVelocityGain();
        settings.velocityIntegratorGain = speedController.getVelocityIntegratorGain();
        configManager.saveSettings(settings);

    }
};

#endif