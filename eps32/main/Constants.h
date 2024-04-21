#ifndef KONSTANTEN_H
#define KONSTANTEN_H

/*
==========================================
==     SETTINGS - HARDWARE              ==
==========================================
*/

// Knopf 1 und 2
#define BUTTON_1 13 // verwendeter GPIO für Button 1
#define BUTTON_2 14 // verwendeter GPIO für Button 2

// Definiere die HALL-Sensoren (Pedale)
const int HALL_BW_PIN = 35; // GPIO35 = HALL2 Backward analoger Input
const int HALL_FW_PIN = 34; // GPIO34 = HALL1 Forward  analoger Input
const float HALL_ANALOG_MAX = 3000; // Analog gelesene Spannung wenn Pedal komplett durchgedrückt wird (3.0V = 3000)
const float HALL_ANALOG_MIN = 540; // Analog gelesene Spannung wenn Pedal nicht betätigt wird (0.54V = 540)



/*
==========================================
==     SETTINGS - CAR                   ==
==========================================
*/

enum ControlModus{
  SPEED_CONTROL = 0;
  TORQUE_CONTROL = 1; 
}


// Struktur für die Parameter der Speedmodi
struct SpeedModusParameter {
  const char* name; // Name des Modus
  int maxSpeed;        // Geschwindigkeitswert
  float maxTorque; // Beschleunigungswert
};

// Definiere die verfügbaren Speedmodi als enum
enum SpeedModus {
  MODUS_1,
  MODUS_2,
  MODUS_3,
  MODUS_4,
  MODUS_R
};

// Definiere die Parameter für jeden Modus
// {str: Name des Modus, int:max Km/h, int:max Drehmoment}
const SpeedModusParameter modiParameter[] = {
  {"1", 5, 2.0},
  {"2", 8, 5.0},
  {"3", 12, 8.0},
  {"4", 16, 999.0},
  {"R", 4, 10.0},
};

const float RADIUSCM = 20.5; // Definiere Reifendurchmesser (Für Geschwindigkeitberechnung in Kmh) in cm

/*
==========================================
==     SETTINGS - DISPLAY               ==
==========================================
*/


#endif
