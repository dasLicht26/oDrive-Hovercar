#ifndef Config_h
#define Config_h 

#include <Arduino.h>
/*
==========================================
==     OPTIONS - HARDWARE - ESP32      ==
==========================================
*/

// GPIO-Pins für Knopf 1 und 2
extern const int BUTTON_1; 
extern const int BUTTON_2; 

// GPIO für Knopf UP/DOWN/OK (Anstelle LED-Pannel, da Display)
extern const int BUTTON_UP; 
extern const int BUTTON_DOWN;  
extern const int BUTTON_OK;

// GPIO-Pins für I2C-Display
extern const int OLED_SDA; 
extern const int OLED_SCL; 

// GPIO-Pins für oDrive UART Kommunikation
extern const int ODRIVE_RX; 
extern const int ODRIVE_TX; 

// GPIO-Pins für HALL Sensoren (Pedale)
extern const int HALL_BW_PIN; 
extern const int HALL_FW_PIN; 

// Definiere ESP32 GPIO-Pins und UART-Schnittstelle zu oDrive
extern const int ODRIVE_BAUD_RATE; 
extern const int ODRIVE_UART; 


/*
==========================================
==     OPTIONS - SOFTWARE  ESP32        ==
==========================================
*/

extern const float RADIUS_WHEEL_CM; // Definiere Reifendurchmesser (Für Geschwindigkeitberechnung in Kmh) in cm

// Konfig HALL-Sensoren (Pedale)
extern const float HALL_ANALOG_MAX; 
extern const float HALL_ANALOG_MIN; 
extern const int HALL_RESOLUTION; 

// Konfig Batterie 
extern const int V_BAT_MAX; 
extern const int V_BAT_MIN; 
extern const int V_BAT_MIN_START; 
extern const float BAT_MAX_CURRENT; 
extern const float BAT_MAX_CURRENT_MARGIN; 

// Errors
struct ODriveErrors {
    int errorCode;
    String source;
};

//Mögliche Kontroll Modi
enum ControlMode { 
    VELOCITY_CONTROL, // Geschwindigkeitssteuerung --> Gibt mit Pedalen eine Zielgeschindigkeit vor, es wird bis dahin beschleunigt
    TORQUE_CONTROL  // Drehmomentsteuerung --> Gibt mit den Pedalen ein Zieldrehmoment vor, Drehmoment wird erhöht, bis Ziel erreicht wird --> Es ist eher wie ein echtes Auto.
};
extern ControlMode STANDARD_CONTROL_MODE;

// Definiere die verfügbaren Speedmodi als enum
enum SpeedMode {
  MODE_1 = 0,
  MODE_2 = 1,
  MODE_3 = 2,
  MODE_4 = 3,
  MODE_R = 4
};
extern SpeedMode STANDARD_SPEED_MODE;

// Struktur für die Parameter der Speedmodi
struct SpeedModeParameter {
  const char* name; // Name des Modus
  float maxSpeed;   // Geschwindigkeitswert
  float maxTorque;  // Drehmoment in Nm
};

// Deklaration des externen Arrays mit den Parametern für die Speedmodi
extern const SpeedModeParameter modiParameter[];


/*
==========================================
==       OPTIONS - Display               ==
==========================================
*/

// Display Konfiguration
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int OLED_RESET;

// Mögliche-Menüzustände
enum MenuState {
    ERROR_LOW_VOLTAGE,  // Geringe Batteriespannung 
    ERROR_ODRIVE,       // oDrive hat Fehler festgestellt
    MENU_MAIN,          // Hauptmenue -> Standardansicht
    MENU_DEBUG,         // Hidden -> Debug Ausgabe 
    MENU_SETTINGS, 
};
extern MenuState STANDARD_MENUE;

struct MenuItems {
    String name;        // Anzeigename
    float current_value;// Aktueller Wert (wird automatisch gesetzt)
    float step;         // Schrittweite
    int digits;         // Anzahl der angezeigten Nachkommastellen
    bool is_adjustable; // ob Wert verstellbar ist
    bool is_active;     // ob Menüpunkt aktiv ist (wird automatisch gesetzt, standard = false)
};

extern const int STANDARD_SETTING_ITEMS_SIZE;
extern MenuItems STANDARD_SETTING_ITEMS[];


extern bool DEBUG_MODE_AKIV;
extern bool LOCAL_DEBUG;

/*
==========================================
==       OPTIONS - oDrive               ==
==========================================
*/

// ODrive.Axis.AxisState
enum ODriveAxisState {
    AXIS_STATE_UNDEFINED                     = 0,
    AXIS_STATE_IDLE                          = 1,
    AXIS_STATE_STARTUP_SEQUENCE              = 2,
    AXIS_STATE_FULL_CALIBRATION_SEQUENCE     = 3,
    AXIS_STATE_MOTOR_CALIBRATION             = 4,
    AXIS_STATE_ENCODER_INDEX_SEARCH          = 6,
    AXIS_STATE_ENCODER_OFFSET_CALIBRATION    = 7,
    AXIS_STATE_CLOSED_LOOP_CONTROL           = 8,
    AXIS_STATE_LOCKIN_SPIN                   = 9,
    AXIS_STATE_ENCODER_DIR_FIND              = 10,
    AXIS_STATE_HOMING                        = 11,
    AXIS_STATE_ENCODER_HALL_POLARITY_CALIBRATION = 12,
    AXIS_STATE_ENCODER_HALL_PHASE_CALIBRATION = 13,
    AXIS_STATE_ANTICOGGING_CALIBRATION       = 14,
};

// ODrive.Controller.ControlMode
enum ODriveControlMode {
    CONTROL_MODE_VOLTAGE_CONTROL             = 0,
    CONTROL_MODE_TORQUE_CONTROL              = 1,
    CONTROL_MODE_VELOCITY_CONTROL            = 2,
    CONTROL_MODE_POSITION_CONTROL            = 3,
};

// ODrive.ComponentStatus
enum ODriveComponentStatus {
    COMPONENT_STATUS_NOMINAL                 = 0,
    COMPONENT_STATUS_NO_RESPONSE             = 1,
    COMPONENT_STATUS_INVALID_RESPONSE_LENGTH = 2,
    COMPONENT_STATUS_PARITY_MISMATCH         = 3,
    COMPONENT_STATUS_ILLEGAL_HALL_STATE      = 4,
    COMPONENT_STATUS_POLARITY_NOT_CALIBRATED = 5,
    COMPONENT_STATUS_PHASES_NOT_CALIBRATED   = 6,
    COMPONENT_STATUS_NUMERICAL_ERROR         = 7,
    COMPONENT_STATUS_MISSING_INPUT           = 8,
    COMPONENT_STATUS_RELATIVE_MODE           = 9,
    COMPONENT_STATUS_UNCONFIGURED            = 10,
    COMPONENT_STATUS_OVERSPEED               = 11,
    COMPONENT_STATUS_INDEX_NOT_FOUND         = 12,
    COMPONENT_STATUS_BAD_CONFIG              = 13,
    COMPONENT_STATUS_NOT_ENABLED             = 14,
    COMPONENT_STATUS_SPINOUT_DETECTED        = 15,
};

// ODrive.ProcedureResult
enum ODriveProcedureResult {
    PROCEDURE_RESULT_SUCCESS                 = 0,
    PROCEDURE_RESULT_BUSY                    = 1,
    PROCEDURE_RESULT_CANCELLED               = 2,
    PROCEDURE_RESULT_DISARMED                = 3,
    PROCEDURE_RESULT_NO_RESPONSE             = 4,
    PROCEDURE_RESULT_POLE_PAIR_CPR_MISMATCH  = 5,
    PROCEDURE_RESULT_PHASE_RESISTANCE_OUT_OF_RANGE = 6,
    PROCEDURE_RESULT_PHASE_INDUCTANCE_OUT_OF_RANGE = 7,
    PROCEDURE_RESULT_UNBALANCED_PHASES       = 8,
    PROCEDURE_RESULT_INVALID_MOTOR_TYPE      = 9,
    PROCEDURE_RESULT_ILLEGAL_HALL_STATE      = 10,
    PROCEDURE_RESULT_TIMEOUT                 = 11,
    PROCEDURE_RESULT_HOMING_WITHOUT_ENDSTOP  = 12,
    PROCEDURE_RESULT_INVALID_STATE           = 13,
    PROCEDURE_RESULT_NOT_CALIBRATED          = 14,
    PROCEDURE_RESULT_NOT_CONVERGING          = 15,
};


// ODrive.MotorType
enum ODriveMotorType {
    MOTOR_TYPE_HIGH_CURRENT                  = 0,
    MOTOR_TYPE_GIMBAL                        = 2,
    MOTOR_TYPE_ACIM                          = 3,
};

// ODrive.Can.Error
enum ODriveCanError {
    CAN_ERROR_NONE                           = 0x00000000,
    CAN_ERROR_DUPLICATE_CAN_IDS              = 0x00000001,
    CAN_ERROR_BUS_OFF                        = 0x00000002,
    CAN_ERROR_LOW_LEVEL                      = 0x00000004,
    CAN_ERROR_PROTOCOL_INIT                  = 0x00000008,
};


#endif
