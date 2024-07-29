#ifndef DisplayManager_h
#define DisplayManager_h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Constants.h"
#include "Bootlogo.h"
#include "ODriveUART.h"
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
      //delay(10);
      display.clearDisplay();
      display.drawBitmap( 0, 0, hoverLogo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // Lade Bootlogo
      display.display();
    }

    // setze Menüzustand (z.B. bei Fehler)
    void setMenuState(MenuState state) {
      currentMenuState = state;
    }

    void displayGenericMessage(const String& message) {
      display.clearDisplay(); // Lösche den aktuellen Inhalt des Displays
      display.setTextSize(1); // Setze die Textgröße. 
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.setCursor(0, 0); // Setze den Cursor an den Anfang des Displays
      display.println(message); 
      display.display(); // Zeige die Änderungen auf dem Display an
    }

    void updateMenu(bool buttonOK, bool buttonUP, bool buttonDOWN, SpeedController& speedController, ConfigManager& configManager, ODriveUART& odrive) {
      static bool lastButtonOK = false;
      static bool lastButtonUP = false;
      static bool lastButtonDOWN = false;

      if (buttonOK && !lastButtonOK) {
        switch (currentMenuState) {
          case MENU_MAIN:
            currentMenuState = MENU_DEBUG;
            break;
          case MENU_DEBUG:
            currentMenuState = MENU_ADJUST_MAIN;
            break;
          case MENU_ADJUST_MAIN:
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
            currentMenuState = MENU_MAIN;
            break;
        }
      }

      if (buttonUP && !lastButtonUP) {
        switch (currentMenuState) {
          case MENU_ADJUST_VEL_GAIN:
            speedController.setVelocityGain(speedController.getVelocityGain() + 0.01);
            break;
          case MENU_ADJUST_VEL_INTEGRATOR_GAIN:
            speedController.setVelocityIntegratorGain(speedController.getVelocityIntegratorGain() + 0.01);
            break;
          case MENU_ADJUST_CONTROL_MODE:
            if (speedController.getControlMode() == VELOCITY_CONTROL) {
                speedController.setControlMode(TORQUE_CONTROL);
            } else {
                speedController.setControlMode(VELOCITY_CONTROL);
            }
            break;
          case MENU_SAVE_SETTINGS:
            saveSettings(speedController, configManager);
            break;
        }
      }

      if (buttonDOWN && !lastButtonDOWN) {
        switch (currentMenuState) {
          case MENU_ADJUST_VEL_GAIN:
            speedController.setVelocityGain(speedController.getVelocityGain() - 0.01);
            break;
          case MENU_ADJUST_VEL_INTEGRATOR_GAIN:
            speedController.setVelocityIntegratorGain(speedController.getVelocityIntegratorGain() - 0.01);
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

      displayMenu(speedController, odrive);
    }

    void displayMenu(SpeedController& speedController, ODriveUART& odrive) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);

      switch (currentMenuState) {
        case MENU_MAIN:
          mode_parameter = speedController.getSpeedModeParameter();
          bat_voltage = odrive.getParameterAsFloat("vbus_voltage");
          bat_percent = map(bat_voltage, V_BAT_MIN, V_BAT_MAX, 0, 100);
          request_kmh = speedController.getRequestedKMH();
          current_kmh = speedController.getCurrentKMH();
          displayDashboard(request_kmh, current_kmh, bat_percent, bat_voltage, mode_parameter.name, mode_parameter.maxSpeed);
          break;
        case MENU_DEBUG:
          display.println("some DEBUG - Menu");
          break;  
        case MENU_ADJUST_MAIN:
          display.println("Settings Menu");
          display.println("1. Vel Gain");
          display.println("2. Vel Int Gain");
          display.println("3. Control Mode");
          display.println("4. Save Settings");
          break;
        case MENU_ADJUST_VEL_GAIN:
          display.println("Adjust Vel Gain");
          display.print("Value: ");
          display.println(speedController.getVelocityGain());
          display.println("UP/DOWN to change");
          display.println("OK to confirm");
          break;
        case MENU_ADJUST_VEL_INTEGRATOR_GAIN:
          display.println("Adjust Vel Int Gain");
          display.print("Value: ");
          display.println(speedController.getVelocityIntegratorGain());
          display.println("UP/DOWN to change");
          display.println("OK to confirm");
          break;
        case MENU_ADJUST_CONTROL_MODE:
          display.println("Adjust Control Mode");
          display.print("Current: ");
          display.println(speedController.getControlMode() == VELOCITY_CONTROL ? "Velocity" : "Torque");
          display.println("UP/DOWN to toggle");
          display.println("OK to confirm");
          break;
        case MENU_SAVE_SETTINGS:
          display.println("Save Settings");
          display.println("Press UP to save!");
          display.println("oDrive will restart");
          break;
        case ERROR_LOW_VOLTAGE:
          bat_voltage = odrive.getParameterAsFloat("vbus_voltage");
          vBatLowError(bat_voltage);
          break;
        case ERROR_ODRIVE:
          std::vector<ODriveErrors> errors = speedController.getErrors();
          for (const auto& error : errors) {
              Serial.print("Error from ");
              Serial.print(error.source.c_str());
              Serial.print(": ");
              Serial.println(error.errorCode);
    }
          displayErrors(errors);
          break;
      }
      display.display();
    }

  private:
    SpeedModeParameter mode_parameter;
    int error_count;
    int bat_percent;
    float bat_voltage;
    float request_kmh;
    float current_kmh;
    MenuState currentMenuState = MENU_MAIN; // setze Standardmenüzustand

    void saveSettings(SpeedController& speedController, ConfigManager& configManager) {
      Settings settings;
      settings.speedMode = speedController.getSpeedMode();
      settings.controlMode = speedController.getControlMode();
      settings.velocityGain = speedController.getVelocityGain();
      settings.velocityIntegratorGain = speedController.getVelocityIntegratorGain();
      configManager.saveSettings(settings);
    }

    void displaySpeedmode(String mode_name){
      display.drawRoundRect (0, 0, 22, 30,5, SSD1306_WHITE);
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.setCursor(3, 5); // Setze den Cursor an den Anfang des Displays
      display.setTextSize(3); // Setze die Textgröße. 
      display.println(mode_name); 
    }

    void displaySpeed(float requestKmh, float currentKmh, int maxKMH){
      if (requestKmh < 0){
        requestKmh = requestKmh * -1;
      }
      if (currentKmh < 0){
        currentKmh = currentKmh * -1;
      }

      display.setCursor(24, 18); // Setze den Cursor hinter 'mode'
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

    void displayBat(int bat_percent, float bat_voltage){
      display.fillRect(115, 0, 6, 2, SSD1306_WHITE);
      display.drawRoundRect (108, 2, 20, 30,3, SSD1306_WHITE);

      if (bat_percent > 80){
        display.fillRect(110, 6, 16, 5, SSD1306_WHITE);
      }
      if (bat_percent > 60){
        display.fillRect(110, 12, 16, 5, SSD1306_WHITE);
      }
      if (bat_percent > 40){
        display.fillRect(110, 18, 16, 5, SSD1306_WHITE);
      }
      if (bat_percent > 20){
        display.fillRect(110, 24, 16, 5, SSD1306_WHITE);
      }
      display.setCursor(108, 34); // Setze den Cursor hinter 'mode'
      display.setTextSize(1); 
      display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
      display.println(String(int(bat_voltage + 0.5)) + "V");
    }

    void vBatLowError(float bat_voltage){
      bool show = false;
      while(true){
        display.clearDisplay(); // Lösche Displayinhalt
        display.setCursor(0, 0); // Setze den Cursor hinter 'mode'
        display.setTextSize(2); 
        display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
        display.println("Batterie");
        display.print("leer");
        if (show == false){
          displayBat(21, bat_voltage);
          show = true;
          }
        else{
          show=false;
          }
        display.display();
      }
    }

    void displayDashboard(float request_kmh, float current_kmh, int bat_percent, float bat_voltage, String mode_name, int max_kmh){
      displaySpeedmode(mode_name); // Setze Geschwindigkeitsmodus (R/1/2/3/4)
      displaySpeed(request_kmh, current_kmh, max_kmh); // Setze Kmh 
      displayBat(bat_percent, bat_voltage);
    }


    void displayErrors(std::vector<ODriveErrors> errors) {
      display.setTextSize(1); // Setze die Textgröße
      int errorCount = errors.size();
      for (const auto& error : errors) {
          display.print("Error from ");
          display.print(error.source.c_str());
          display.print(": ");
          display.println(error.errorCode);
      }
    }
};

#endif