#include "HallSensor.h"
#include "DisplayManager.h"
#include "SpeedControl.h"

#include "ODriveUART.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

// Knopf 1 und 2
#define BUTTON_1 13 // verwendeter GPIO für Button 1
#define BUTTON_2 14 // verwendeter GPIO für Button 2

// Definiere die HALL-Sensoren (Pedale)
const int HALL_BW_PIN = 35; // GPIO35 = HALL2 Backward analoger Input
const int HALL_FW_PIN = 34; // GPIO34 = HALL1 Forward  analoger Input
const float HALL_ANALOG_MAX = 3000; // Analog gelesene Spannung wenn Pedal komplett durchgedrückt wird (3.0V = 3000)
const float HALL_ANALOG_MIN = 540; // Analog gelesene Spannung wenn Pedal nicht betätigt wird (0.54V = 540)

// Angaben zur Geschwindigkeitsberechnung und überwachung
const float RADIUSCM = 20.5; // Definiere Reifendurchmesser (Für Geschwindigkeitberechnung in Kmh) in cm
const int MAX_KMH_MODE_R = 5; // Maximalgeschwindigkeit in KM/h beim Rückwertsfahren --> (Errechnet sich aus Reifendurchmesser RADIUSCM)
const int MAX_KMH_MODE_1 = 5; // Maximalgeschwindigkeit in KM/h im Modus 1 --> (Errechnet sich aus Reifendurchmesser RADIUSCM)
const int MAX_KMH_MODE_2 = 10; // Maximalgeschwindigkeit in KM/h im Modus 2 --> (Errechnet sich aus Reifendurchmesser RADIUSCM) 
const int MAX_KMH_MODE_3 = 18;// Maximalgeschwindigkeit in KM/h im Modus 3 --> (Errechnet sich aus Reifendurchmesser RADIUSCM)
const int MAX_KMH_MODE_4 = 30; // Maximalgeschwindigkeit in KM/h im Modus 4 --> (Errechnet sich aus Reifendurchmesser RADIUSCM) 
String speedMode; // Gesetzter Geschwindigkeitesmodus -> Wert wird beim Einschalten/Booten gesetzt (setSpeedMode())
int maxKmh; // Gesetzte Maximalgeschwindigkeit in Km/h -> Wert wird beim Einschalten/Booten gesetzt (setSpeedMode())

// angaben zur Batterie (Angaben für 36V Lipo-Akkus 10S2P)
const int V_BAT_MAX = 41; // 41 Volt = 100% Ladung der Akkus, Werte darüber werden auf 41 abgeschnitten.
const int V_BAT_MIN = 32; // 32 Volt = 0% Ladung des Akkus, das HoverCar nimmt kein Gas mehr an wenn es bereits eingeschalten ist (Es bremmst auf 0).
const int V_BAT_MIN_START = 34; // Minimale Akkuladung um odrive in AXIS_STATE_CLOSED_LOOP_CONTROL zu versetzen. (darunter lässt sich das HooverCar nicht mehr einschalten)

// Definiere ESP32 GPIO-Pins und UART-Schnittstelle
const int ODRIVE_BAUD_RATE = 115200; // Baudrate zur Kommunikation mit oDrive (115200)
const int ODRIVE_UART = 1; // verwendeter UART-Bus des ESP32 (UART1)
const int ODRIVE_RX = 10; // verwendeter GPIO für RX UART-Bus des ESP32 (GPIO-10)
const int ODRIVE_TX = 9; // verwendeter GPIO für TX UART-Bus des ESP32 (GPIO-9)

// bugfixing
int countLoop = 0;

HallSensor sensor;  //initialisiere HALL-Sensoren (Gas/Bremse)
DisplayManager displayManager; //initialisiere OLED-Display
SpeedControler speedControler; //initialisiere Geschwinigkeitskontrolle
HardwareSerial odrive_serial(ODRIVE_UART); // Verwender UART im ESP32
ODriveUART odrive(odrive_serial);


void setSpeedMode(){
  pinMode(BUTTON_1, INPUT_PULLUP); 
  pinMode(BUTTON_2, INPUT_PULLUP); 
  int buttonOneState = digitalRead(BUTTON_1);
  int buttonTwoState = digitalRead(BUTTON_2);

  if(buttonOneState == LOW && buttonTwoState == LOW){
    speedMode = "4";
    maxKmh = MAX_KMH_MODE_4;
  }
  else if (buttonOneState == LOW){
    speedMode = "2";
    maxKmh = MAX_KMH_MODE_2;
  }
  else if (buttonTwoState == LOW){
    speedMode = "3";
    maxKmh = MAX_KMH_MODE_3;
  }
  else {
    speedMode = "1";
    maxKmh = MAX_KMH_MODE_1;
  }
}

void setup() {
    // Setup Controller
    Serial.begin(115200); // starte seriellen Monitor zu ESP32
    odrive_serial.begin(ODRIVE_BAUD_RATE, SERIAL_8N1, ODRIVE_RX, ODRIVE_TX); // starte serielle Verbindung zum odrive
    setSpeedMode(); // setze Geschwindigkeitsbegrenzungen 

    sensor.setup(MAX_HALL_VALUE , MIN_HALL_VALUE); // HALL Sensoren (Pedale)
    displayManager.setup(MAX_HALL_VALUE , MIN_HALL_VALUE); // Display
    speedControler.setup(MAX_HALL_VALUE , MIN_HALL_VALUE, RADIUSCM); // Geschwindigkeit Outputkontrolle
    

    displayManager.displayDashboard(16.56, 22.88, 81, speedMode);

    delay(10);

  // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive.getState(0) == AXIS_STATE_UNDEFINED) {
      delay(30);
    }

    // setze Axis0 in Closed_loop (externen Steuerungsmodus)
    while (odrive.getState(0) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
      odrive.clearErrors();
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 0);
      delay(10);
    }

      // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive.getState(1) == AXIS_STATE_UNDEFINED) {
      delay(30);
    }

    // setze Axis0 in Closed_loop (externen Steuerungsmodus)
    while (odrive.getState(1) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
      odrive.clearErrors();
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 1);
      delay(10);
    }

    Serial.println("ODrive running!");
  }


void loop() {
  // leere seriellen Buffer (ggf. unnötig?)
  odrive_serial.flush();
  Serial.flush();

  // setze odrive Watchdog-Timer zurück, wenn dieser nicht alle 0.8 Sekunden zurückgesetzt wird, gehen die Motoren in Notaus (Falls es ein Verbindungsabbruch gibt)
  odrive.resetWatchdog(0); // Setze Watchdog Axis 0 zurück (odrive geht sonst nach 0.8sek in Notaus)
  odrive.resetWatchdog(1); // Setze Watchdog Axis 0 zurück (odrive geht sonst nach 0.8sek in Notaus)

  // sammle HALL Werte (Pedale)
  int backwardValue = sensor.readValue(HALL_BW_PIN);
  int forwardValue = sensor.readValue(HALL_FW_PIN);

  // lese Batteriespannung
  float vBusVoltage =  odrive.getParameterAsFloat("vbus_voltage");

  // lese odrive, aktuelle Umdrehungen pro Sekunde der Räder
  float odriveRPS0 =  odrive.getParameterAsFloat("axis0.encoder.vel_estimate");
  float odriveRPS1 =  odrive.getParameterAsFloat("axis0.encoder.vel_estimate");


  int inputSpeedValueInt = speedControler.getSpeed(forwardValue, backwardValue, 0);
  float inputSpeedValue = static_cast<float>(inputSpeedValueInt) * 0.001;



  float speedKMh = speedControler.speedKMh(odriveVel);


  displayManager.displayMessage(speedKMh);
  // Gesetzte Geschwindigkeit

  //odrive.setVelocity(inputSpeedValue, 0);

  
  Serial.print("KM/H: ");
  Serial.println(speedKMh);

  Serial.print(" : ");
  Serial.println(vBusVoltage);
  
  //delay(0);
}

