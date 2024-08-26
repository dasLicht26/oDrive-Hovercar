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

    // nimm Knopfzustände entgegen und aktualisiere Menü entsprechend
    void handleInput(bool button_ok, bool button_up, bool button_down);

    // setze speedController für DisplayManager
    void setSpeedController(SpeedController* speedController) { this->speedController = speedController; }

private:
    SpeedController* speedController;   // Zeiger auf SpeedController
    Adafruit_SSD1306& display;          // Zeiger auf Display-Objekt
    MenuState current_menu_state;         // Aktueller Menüzustand
    SpeedModeParameter mode_parameter;  // Parameter des aktuellen Geschwindigkeitsmodus
    std::vector<MenuItem> menu_settings_items;    // verfügbare Menüpunkte der Einstellungen
    std::vector<ODriveErrors> errors;   // ausgelesene Fehlerliste aus oDrive
    char last_button;            // zuletzt gedrückter Knopf (o, +, -) oder n für nichts, um Doppelklicks zu vermeiden
    char button_pressend;        // aktuell gedrückter Knopf
    int menu_settings_state;            // aktuell ausgewählter Menüpunkt in Settings
    int menu_state;                     // aktuell ausgewählter Menüpunkt
    int bat_percent;                    // Batterieprozentsatz      
    bool low_voltage_blink;             // ob Batterie leer Fehler blinken soll
    bool settings_active;               // ob Einstellungs-menü aktiv sind
    float bat_voltage;                  // Batteriespannung
    float current_kmh;                  // aktuelle Geschwindigkeit in km/h    
    float requested_kmh;                // angeforderte Geschwindigkeit in km/h

    // aktualisiere Menüzustand entsprechned der Knopfzustände
    void updateMenuState();

    // aktualisiere Menüpunkt Settings entsprechend der Knopfzustände
    void updateMenuItem();

    // zeige Einstellungen-Menü an
    void displaySettingsMenu();

    // zeige Batterie-Icon an -> Prozent wird automatisch berechnet
    void displayBat();

    // zeige Batterie-Icon an -> Prozent wird übergeben
    void displayBat(int bat_percent);

    // zeige Geschwindigkeits-Anzeige an
    void displaySpeed();

    // zeige Geschwindigkeitsmodus an
    void displaySpeedmode();

    // zeige komplettes Dashboard an (Geschwindigkeit, Batterie, Geschwindigkeitsmodus)
    void displayDashboard();

    // zeige Batterie leer Fehler an
    void displayVBatLowError();

    // zeige Debug-Menü an
    void displayDebugMenu();

    // zeige Fehler an
    void displayErrors();
};

#endif // DISPLAY_MANAGER_H
