#include "DisplayManager.h"

DisplayManager::DisplayManager(Adafruit_SSD1306& display)
    : display(display), selectedItem(0), in_edit_mode(false) {
    Wire.begin(OLED_SDA, OLED_SCL); // Initialisiere I2C mit SDA und SCL Pins
    Wire.setClock(50000); // Setzt die Taktrate auf 50 kHz (standard ist 100kHz) für stabielere Bilder
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 allocation failed"));
            for (;;);
        }
    }

void DisplayManager::displayBootlogo() {
    display.clearDisplay();
    display.drawBitmap( 0, 0, hoverLogo, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // Lade Bootlogo
    display.display();
}

// setze aktuellen Menüzustand (z.B. bei Fehler)
void DisplayManager::setMenuState(MenuState state) {
    currentMenuState = state;
}

void DisplayManager::handleInput(char input) {
    static bool lastButtonOK = false;
    static bool lastButtonUP = false;
    static bool lastButtonDOWN = false;
}

void DisplayManager::updateDisplay() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    if(speedController == nullptr) {
        display.println("Display Error!");
        display.print("SpeedController not set!"); 
        return;
    }

    // Lese aktuelle Werte von SpeedController
    mode_parameter = speedController->getSpeedModeParameter();
    bat_voltage = speedController->getBatteryVoltage();
    bat_percent = speedController->getBatteryPercentage();
    requested_kmh = speedController->getRequestedKMH();
    current_kmh = speedController->getCurrentKMH();

    switch (currentMenuState) {
    case MENU_MAIN:
        displayDashboard();
        break;
    case MENU_DEBUG:
        displayDebugMenu();
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



    // Erstelle den anzuzeigenden String
    String displayString = 
            "Speed: " + String(current_kmh, 2) + " km/h\n" +
            "Strom: " + String(speedController->getVBusCurrent(), 2) + " A\n" +
            "Drehmoment: " + String(speedController->getCurrentNM(), 2) + " Nm\n" +
            "angef_kmh: " + String(requested_kmh, 2) + " km/h\n" +
            "angef_NM: " + String(speedController->getRequestedNm(), 2) + " Nm\n";

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
