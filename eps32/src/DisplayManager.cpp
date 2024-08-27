#include "DisplayManager.h"
#include "Config.h"


void DisplayManager::displayBootlogo() {
    display.clearDisplay();
    display.drawBitmap( 0, 0, hoverLogo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // Lade Bootlogo
    display.display();
    delay(2000); // Zeige Bootlogo für 2 Sekunden
}

// setze aktuellen Menüzustand (z.B. bei Fehler)
void DisplayManager::setMenuState(MenuState state) {
    current_menu_state = state;
}

void DisplayManager::handleInput(bool button_ok, bool button_up, bool button_down) {

    if (button_ok && !button_up && !button_down) {
        button_pressed = 'o';
    } else if (!button_ok && button_up && !button_down) {
        button_pressed = '+';
    } else if (!button_ok && !button_up && button_down) {
        button_pressed = '-';
    } else {
        button_pressed = 'n';
    }

    if (button_pressed == last_button) {
        return;
    } else {
        last_button = button_pressed;
    }
    
    if (button_pressed == 'n') {
        return;
    }

    updateMenuItem();
    updateMenuState();
    

}

void DisplayManager::updateMenuState() {
    if (settings_active) {
        return;
    }

    if (button_pressed == '+') {
        if (current_menu_state == MENU_MAIN) {
            current_menu_state = MENU_DEBUG;
        } else if (current_menu_state == MENU_DEBUG) {
            current_menu_state = MENU_SETTINGS;
        } else if (current_menu_state == MENU_SETTINGS) {
            current_menu_state = MENU_MAIN;
        }
    } else if (button_pressed == '-') {
        if (current_menu_state == MENU_MAIN) {
            current_menu_state = MENU_SETTINGS;
        } else if (current_menu_state == MENU_DEBUG) {
            current_menu_state = MENU_MAIN;
        } else if (current_menu_state == MENU_SETTINGS) {
            current_menu_state = MENU_DEBUG;
        }
    } else if (button_pressed == 'o') {
        if (current_menu_state == MENU_SETTINGS) {
            settings_active = true;
            menu_settings_state = 0;
        }
    }
}

void DisplayManager::updateMenuItem() {
    if(!settings_active) {
        return;
    }

    if (current_menu_state == MENU_SETTINGS && !STANDARD_SETTING_ITEMS[menu_settings_state].is_active) {
        if (button_pressed == '-') {
            menu_settings_state++;
            if (menu_settings_state >= STANDARD_SETTING_ITEMS_SIZE) {
                menu_settings_state = 0;
            }
        } else if (button_pressed == '+') {
            menu_settings_state--;
            if (menu_settings_state < 0) {
                menu_settings_state = STANDARD_SETTING_ITEMS_SIZE - 1;
            }
        }
    }
    if (button_pressed == 'o') {
        if (STANDARD_SETTING_ITEMS[menu_settings_state].is_active) {
        STANDARD_SETTING_ITEMS[menu_settings_state].is_active = false;
        } else {
        STANDARD_SETTING_ITEMS[menu_settings_state].is_active = true;
        }

        if (!STANDARD_SETTING_ITEMS[menu_settings_state].is_adjustable) {
            settings_active = false;
            STANDARD_SETTING_ITEMS[menu_settings_state].is_active = false;
            button_pressed = 'n';
            if (STANDARD_SETTING_ITEMS[menu_settings_state].name == "Save Settings") {
                current_menu_state = MENU_MAIN;

                /* ToDo: Speichere die Einstellungen in oDrive */
            } else {
                current_menu_state = MENU_SETTINGS;
                /* ToDo: Verwerfe die Einstellungen */

            }
        }   
    }
}

void DisplayManager::displaySettingsMenu() {

    int16_t x1, y1;
    uint16_t w, h;

    for (int i = 0; i < STANDARD_SETTING_ITEMS_SIZE; i++) {
        // Cursor an den Anfang der Zeile setzen
        display.setCursor(0, i * 8);  // Annahme: Jede Zeile ist 8 Pixel hoch, passe dies entsprechend an

        // Wenn Settings-Menü aktiv ist, dann zeige ">" an
        if (i == menu_settings_state && settings_active) {
            if(STANDARD_SETTING_ITEMS[i].is_active) {
                display.print(">>");
            } else {
                display.print("> ");
            }
        } else {
            display.print("  ");
        }

        // Anpassen wenn Menüpunkt aktiv ist
        if (STANDARD_SETTING_ITEMS[i].is_active) {
            display.setTextColor(SSD1306_WHITE);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }

        // Wenn Menüpunkt verstellbar ist/ein Wert besitzt, dann zeige Wert an
        if (STANDARD_SETTING_ITEMS[i].is_adjustable) {
            // Name und ": " linksbündig ausgeben
            display.print(STANDARD_SETTING_ITEMS[i].name);
            display.print(": ");

            // Breite des Wertes berechnen
            display.getTextBounds(String(STANDARD_SETTING_ITEMS[i].current_value), 0, 0, &x1, &y1, &w, &h);

            // X-Position berechnen, um den Wert rechtsbündig auszugeben
            int16_t xPos = display.width() - w;

            // Cursor für den Wert setzen
            display.setCursor(xPos, i * 8);  // Die gleiche Y-Position wie oben verwenden

            // Wert rechtsbündig ausgeben
            display.println(STANDARD_SETTING_ITEMS[i].current_value);
        } else if (settings_active) {
            // Wenn Menüpunkt nicht verstellbar ist, dann zeige nur den Namen an
            display.println(STANDARD_SETTING_ITEMS[i].name);
        } 
    }
    if (!settings_active) {
        display.println("");
        display.println("Bearbeiten mit 'OK'");

    }


    //display.display();
}

void DisplayManager::updateDisplay() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    if(speedController == nullptr) {
        display.println("Display Error!");
        display.print("SpeedController not set!"); 
        display.display();
        return;
    }

    // Lese aktuelle Werte von SpeedController
    if (!LOCAL_DEBUG){
        mode_parameter = speedController->getSpeedModeParameter();
        bat_voltage = speedController->getBatteryVoltage();
        bat_percent = speedController->getBatteryPercentage();
        requested_kmh = speedController->getRequestedKMH();
        current_kmh = speedController->getCurrentKMH();
    } else {
        mode_parameter = speedController->getSpeedModeParameter();
        bat_voltage = 36.0;
        bat_percent = 68;
        requested_kmh = 12.3;
        current_kmh = 9.4;
    }

    switch (current_menu_state) {
    case MENU_MAIN:
        displayDashboard();
        break;
    case MENU_DEBUG:
        displayDebugMenu();
        break;  
    case MENU_SETTINGS:
        displaySettingsMenu();
        break;
    case ERROR_LOW_VOLTAGE:
        displayVBatLowError();
        break;
    case ERROR_ODRIVE:
        displayErrors();
        break;
    }
    display.display();
}

void DisplayManager::displayDebugMenu() {
    if(speedController == nullptr) {
        display.println("Display Error!");
        display.print("SpeedController not set!"); 
        return;
    }

    String displayString;
    if (!LOCAL_DEBUG){    // Erstelle den anzuzeigenden String
        displayString = 
            "Speed: " + String(current_kmh, 2) + " km/h\n" +
            "Strom: " + String(speedController->getVBusCurrent(), 2) + " A\n" +
            "Drehmoment: " + String(speedController->getCurrentNM(), 2) + " Nm\n" +
            "angef_kmh: " + String(requested_kmh, 2) + " km/h\n" +
            "angef_NM: " + String(speedController->getRequestedNm(), 2) + " Nm\n";
    } else {
        displayString = 
            "Speed: " + String(current_kmh, 2) + " km/h\n" +
            "Strom: " + String(11.2) + " A\n" +
            "Drehmoment: " + String(8.4, 2) + " Nm\n" +
            "angef_kmh: " + String(requested_kmh, 2) + " km/h\n" +
            "angef_NM: " + String(9.1, 2) + " Nm\n";
    }

    display.println(displayString);
}

void DisplayManager::displaySpeedmode(){
    display.drawRoundRect (0, 0, 22, 30,5, SSD1306_WHITE); 
    display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe 
    display.setCursor(3, 5); // Setze den Cursor an den Anfang des Displays 
    display.setTextSize(3); // Setze die Textgröße. 
    display.println(mode_parameter.name); 
}

void DisplayManager::displaySpeed(){
    if (requested_kmh < 0){
    requested_kmh = requested_kmh * -1;
    }
    if (current_kmh < 0){
    current_kmh = current_kmh * -1;
    }

    display.setCursor(24, 18); // Setze den Cursor hinter 'mode'
    display.setTextSize(7); 
    int current_kmh_int = current_kmh + 0.5;
    String current_kmh_str;
    if (current_kmh_int < 10){
    current_kmh_str = "0" + String(current_kmh_int);
    }
    else {
    current_kmh_str = String(current_kmh_int);
    }
    display.print(current_kmh_str); 
}

void DisplayManager::displayBat(){
    displayBat(bat_percent);
}

void DisplayManager::displayBat(int bat_percent){
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

void DisplayManager::displayVBatLowError(){
    display.clearDisplay(); // Lösche Displayinhalt
    display.setCursor(0, 0); // Setze den Cursor hinter 'mode'
    display.setTextSize(2); 
    display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
    display.println("Batterie");
    display.print("leer");
    if (low_voltage_blink == false){
    displayBat(21);
    low_voltage_blink = true;
    }
    else{
    low_voltage_blink=false;
    }
    display.display();
    delay(1000);
}

void DisplayManager::displayDashboard(){
    displaySpeedmode(); // Setze Geschwindigkeitsmodus (R/1/2/3/4)
    displaySpeed(); // Setze Kmh 
    displayBat();
}


void DisplayManager::displayErrors() {
    if(speedController == nullptr) {
        display.println("Display Error!");
        display.print("SpeedController not set!"); 
        return;
    }

    errors = speedController->getErrors();
    display.setTextSize(1); // Setze die Textgröße
    int errorCount = errors.size();
    for (int i = 0; i < errorCount; i++) {
    display.setCursor(0, i * 10);
    display.print("Error ");
    display.print(errors[i].source.c_str());
    display.print(": ");
    display.println(errors[i].errorCode);
    }
}
