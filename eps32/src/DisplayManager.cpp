#include "DisplayManager.h"
#include "Config.h"


void DisplayManager::displayBootlogo() {
    display.clearDisplay();
    display.drawBitmap( 0, 0, hoverLogo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // Lade Bootlogo
    display.display();
    delay(1000); // Zeige Bootlogo für 2 Sekunden
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

    // Reihenfolge von updateMenueItem und updateMenuState nicht ändern!!
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
    // Einstellungsmenü ist nicht aktiv -> aktuallisiere/resette die Einstellungen
    if(!settings_active) {
        STANDARD_SETTING_ITEMS[0].current_value = speedController->getVelocityGain();
        STANDARD_SETTING_ITEMS[1].current_value = speedController->getVelocityIntegratorGain();
        STANDARD_SETTING_ITEMS[2].current_value = speedController->getTorqueMinimum();
        STANDARD_SETTING_ITEMS[3].current_value = speedController->getTorqueSlope();
        return;
    }

    // Einstellungsmenü ist aktiv und Menüpunkt Settings ist auch aktiv (eigentlich überflüssig)
    if (current_menu_state == MENU_SETTINGS) {
        // - wird gedrückt
        if (button_pressed == '-') {
            // Wenn Menüpunkt aktuell NICHT bearbeitet wird, schalte durch das Menü (runter)
            if (!STANDARD_SETTING_ITEMS[menu_settings_state].is_active){
                menu_settings_state++;
                if (menu_settings_state >= STANDARD_SETTING_ITEMS_SIZE) {
                    menu_settings_state = 0;
                }
            } else {
                // Wenn Menüpunkt verstellbar ist und aktuell bearbeitet wird, dann verändere diesen Wert
                STANDARD_SETTING_ITEMS[menu_settings_state].current_value -= STANDARD_SETTING_ITEMS[menu_settings_state].step;
                if (STANDARD_SETTING_ITEMS[menu_settings_state].current_value < 0.0) {
                    STANDARD_SETTING_ITEMS[menu_settings_state].current_value = 0.0;
                }
            }
        // + wird gedrückt
        } else if (button_pressed == '+') {
            if (!STANDARD_SETTING_ITEMS[menu_settings_state].is_active){
                menu_settings_state--;
                if (menu_settings_state < 0) {
                    menu_settings_state = STANDARD_SETTING_ITEMS_SIZE - 1;
                }
            } else {
                // Wenn Menüpunkt verstellbar ist und aktuell bearbeitet wird, dann verändere diesen Wert
                STANDARD_SETTING_ITEMS[menu_settings_state].current_value += STANDARD_SETTING_ITEMS[menu_settings_state].step;
            }
        } 
    }
    // ok wird gedrückt
    if (button_pressed == 'o') {
        // setze ausgewählten Menüpunkt is_aktiv true/false (aktiv = Einstellungen können für diesen Punkt geändert werden. +/- ändert den Wert, nicht mehr das Menü)
        if (STANDARD_SETTING_ITEMS[menu_settings_state].is_active) {
        STANDARD_SETTING_ITEMS[menu_settings_state].is_active = false;
        } else {
        STANDARD_SETTING_ITEMS[menu_settings_state].is_active = true;
        }
        // Wenn Menüpunk nicht is_adjustabe ist, dann ist dies Speichern oder Abbrechen. Wird dies mit 'ok' gewählt reagiere entsprechend. 
        if (!STANDARD_SETTING_ITEMS[menu_settings_state].is_adjustable) {
            settings_active = false;
            STANDARD_SETTING_ITEMS[menu_settings_state].is_active = false;
            button_pressed = 'n';
            if (STANDARD_SETTING_ITEMS[menu_settings_state].name == "Save Settings") { // --> Save Settings
                current_menu_state = MENU_SETTINGS;
                speedController->saveODriveConfig();
            } else{ // --> Cancel
                setMenuState(MENU_MAIN);
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
            display.getTextBounds(String(STANDARD_SETTING_ITEMS[i].current_value, STANDARD_SETTING_ITEMS[i].digits), 0, 0, &x1, &y1, &w, &h);
            

            // X-Position berechnen, um den Wert rechtsbündig auszugeben
            int16_t xPos = display.width() - w;

            // Cursor für den Wert setzen
            display.setCursor(xPos, i * 8);  // Die gleiche Y-Position wie oben verwenden

            // Wert rechtsbündig ausgeben
            display.println(STANDARD_SETTING_ITEMS[i].current_value, STANDARD_SETTING_ITEMS[i].digits);
   
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
    display.setTextSize(2);

    int line = 0;
    int line_distance = 16; // abstabd in Pixel zwischen den Zeilen
    int digits = 1; //nachkomma stellen
    int16_t x1, y1;
    uint16_t w, h;
    float current_a;
    float current_nm;
    float current_v;
    float current_kmh;
    float current_rps;

    if(speedController == nullptr) {
        display.println("Display Error!");
        display.print("SpeedController not set!"); 
        return;
    }
    if (!LOCAL_DEBUG){
        current_a = speedController->getVBusCurrent();
        current_nm = speedController->getCurrentNM();
        current_v = speedController->getBatteryVoltage();
        current_kmh = speedController->getCurrentKMH();
        current_rps = speedController->getCurrentVelocity();
    } else {
        current_a = 11.2;
        current_nm = 8.4;
        current_v = 36.0;
        current_kmh = 9.1;
    }
    // Ampere
    display.setCursor(0, line*line_distance);  // Annahme: Jede Zeile ist 8 Pixel hoch, passe dies entsprechend an
    display.print("Ampere:");
    display.getTextBounds(String(current_a, digits), 0, 0, &x1, &y1, &w, &h); // Breite des Wertes berechnen
    int16_t xPos = display.width() - w; // X-Position berechnen, um den Wert rechtsbündig auszugeben
    display.setCursor(xPos, line * line_distance);  // Die gleiche Y-Position wie oben verwenden
    display.println(current_a, digits); // Wert rechtsbündig ausgeben
    // Nm
    line++;
    display.setCursor(0, line*line_distance);  // Annahme: Jede Zeile ist 8 Pixel hoch, passe dies entsprechend an
    display.print("Nm:");
    display.getTextBounds(String(current_nm, digits), 0, 0, &x1, &y1, &w, &h); // Breite des Wertes berechnen
    xPos = display.width() - w; // X-Position berechnen, um den Wert rechtsbündig auszugeben
    display.setCursor(xPos, line * line_distance);  // Die gleiche Y-Position wie oben verwenden
    display.println(current_nm, digits); // Wert rechtsbündig ausgeben
    // Volt
    line++;
    display.setCursor(0, line*line_distance);  // Annahme: Jede Zeile ist 8 Pixel hoch, passe dies entsprechend an
    display.print("VBat:");
    display.getTextBounds(String(current_v, digits), 0, 0, &x1, &y1, &w, &h); // Breite des Wertes berechnen
    xPos = display.width() - w; // X-Position berechnen, um den Wert rechtsbündig auszugeben
    display.setCursor(xPos, line * line_distance);  // Die gleiche Y-Position wie oben verwenden
    display.println(current_v, digits); // Wert rechtsbündig ausgeben
    // Kmh
    line++;
    display.setCursor(0, line*line_distance);  // Annahme: Jede Zeile ist 8 Pixel hoch, passe dies entsprechend an
    display.print("RPS:");
    display.getTextBounds(String(current_rps, digits), 0, 0, &x1, &y1, &w, &h); // Breite des Wertes berechnen
    xPos = display.width() - w; // X-Position berechnen, um den Wert rechtsbündig auszugeben
    display.setCursor(xPos, line * line_distance);  // Die gleiche Y-Position wie oben verwenden
    display.println(current_rps, digits); // Wert rechtsbündig ausgeben


}

void DisplayManager::displaySpeedmode(){
    if (speedController -> getMotorActive() == false){
        display.fillRoundRect(2, 0, 21, 30,5, SSD1306_WHITE); 
        display.setTextColor(SSD1306_BLACK); // Setze die Textfarbe 
        display.setCursor(5, 5); // Setze den Cursor an den Anfang des Displays 
        display.setTextSize(3); // Setze die Textgröße. 
        display.println(mode_parameter.name); 
        display.setTextColor(SSD1306_WHITE);
    }
    else{
    display.drawRoundRect (2, 0, 21, 30,5, SSD1306_WHITE); 
    display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe 
    display.setCursor(5, 5); // Setze den Cursor an den Anfang des Displays 
    display.setTextSize(3); // Setze die Textgröße. 
    display.println(mode_parameter.name); 
    }
}

void DisplayManager::displaySpeed(){
    if (requested_kmh < 0){
    requested_kmh = requested_kmh * -1;
    }
    if (current_kmh < 0){
    current_kmh = current_kmh * -1;
    }

    display.setCursor(32, 4); // Setze den Cursor hinter 'mode'
    display.setTextSize(8); 
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
    display.fillRect(9, 32, 6, 2, SSD1306_WHITE);
    display.drawRoundRect (2, 34, 20, 30,3, SSD1306_WHITE);

    if (bat_percent > 80){
    display.fillRect(4, 38, 16, 5, SSD1306_WHITE);
    }
    if (bat_percent > 60){
    display.fillRect(4, 44, 16, 5, SSD1306_WHITE);
    }
    if (bat_percent > 40){
    display.fillRect(4, 50, 16, 5, SSD1306_WHITE);
    }
    if (bat_percent > 20){
    display.fillRect(4, 56, 16, 5, SSD1306_WHITE);
    }
    display.setCursor(3, 36); // Setze den Cursor hinter 'mode'
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
    //display.println(String(int(bat_voltage + 0.5)) + "V");
}

void DisplayManager::displayVBatLowError(){
    display.clearDisplay(); // Lösche Displayinhalt
    display.setCursor(10, 6); // Setze den Cursor hinter 'mode'
    display.setTextSize(2); 
    display.setTextColor(SSD1306_WHITE); // Setze die Textfarbe
    display.println("Akku leer"); // Setze Text
    //display.print("   leer");
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
