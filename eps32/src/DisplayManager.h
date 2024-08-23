#ifndef DisplayManager_h
#define DisplayManager_h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Bootlogo.h"
#include "ODriveUART.h"
#include "SpeedController.h"
#include "EEPROMSettings.h"
#include <Arduino.h>
#include <vector>



class DisplayManager {
public:
    DisplayManager(Adafruit_SSD1306& display);

    // zeige Bootlogo
    void displayBootlogo();

    // setze aktuellen Menüzustand (z.B. bei Fehler)
    void setMenuState(MenuState state);

    // aktualisiere das Display
    void updateDisplay();

    // nimm Knopfzustände entgegen und aktualisiere Menü
    void handleInput(char input);

    // setze speedController für DisplayManager
    void setSpeedController(SpeedController* speedController) { this->speedController = speedController; }

private:
    SpeedController* speedController;
    Adafruit_SSD1306& display;
    MenuState currentMenuState;
    SpeedModeParameter mode_parameter;
    std::vector<MenuItem> menuItems;
    std::vector<ODriveErrors> errors;
    int selectedItem;
    int bat_percent;
    bool in_edit_mode;
    bool low_voltage_blink;
    float bat_voltage;
    float current_kmh;
    float requested_kmh;

    void displayBat();
    void displayBat(int bat_percent);
    void displaySpeed();
    void displaySpeedmode();
    void displayDashboard();
    void displayVBatLowError();
    void displayDebugMenu();
    void displayErrors();
};

#endif // DISPLAY_MANAGER_H
