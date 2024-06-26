#include "DisplayManager.h"
#include "SpeedControl.h"
#include "Constants.h"

#include "ODriveUART.h"
#include <Arduino.h>
#include <SoftwareSerial.h>



String speedMode; // Gesetzter Geschwindigkeitesmodus -> Wert wird beim Einschalten/Booten gesetzt (setSpeedMode())
int maxKmh; // Gesetzte Maximalgeschwindigkeit in Km/h -> Wert wird beim Einschalten/Booten gesetzt (setSpeedMode())
bool idleMode = false; // ob sich odive im idlemode befindet 


DisplayManager displayManager; //initialisiere OLED-Display
SpeedController speedController; //initialisiere Geschwinigkeitskontrolle
HardwareSerial odrive_serial(ODRIVE_UART); // Verwender UART im ESP32
ODriveUART odrive(odrive_serial);

void setSpeedMode(){
  pinMode(BUTTON_1, INPUT_PULLUP); 
  pinMode(BUTTON_2, INPUT_PULLUP); 
  int buttonOneState = digitalRead(BUTTON_1);
  int buttonTwoState = digitalRead(BUTTON_2);

  if(buttonOneState == LOW && buttonTwoState == LOW){
    speedController.setSpeedMode(MODE_4);
  }
  else if (buttonOneState == LOW){
    speedController.setSpeedMode(MODE_2);
  }
  else if (buttonTwoState == LOW){
    speedController.setSpeedMode(MODE_3);
  }
  else {
    speedController.setSpeedMode(MODE_1);
  }
}

void setControlMode(){
    if(speedController.getHallMappedValue(HALL_FW_PIN) > 50){
      speedController.setControlMode(TORQUE_CONTROL);
    }
    else{
      speedController.setControlMode(SPEED_CONTROL);
    }
  }

void setup() {
    // Setup Controller
    Serial.begin(115200); // starte seriellen Monitor zu ESP32
    odrive_serial.begin(ODRIVE_BAUD_RATE, SERIAL_8N1, ODRIVE_RX, ODRIVE_TX); // starte serielle Verbindung zum odrive
    setSpeedMode(); // setze Geschwindigkeitsbegrenzungen 
    displayManager.setup();

    //displayManager.vBatLowError();
    //displayManager.displayDashboard(16.56, 22.88, 81, speedMode);


    while(odrive.getParameterAsFloat("vbus_voltage") == 0.0){
      delay(30);
    }
    odrive.clearErrors();
    float vel_gain = 3.0;
    float vel_integ_gain = 2.01;

    odrive.setParameter("axis0.controller.config.vel_gain", vel_gain);
    odrive.setParameter("axis1.controller.config.vel_gain", vel_gain);
    odrive.setParameter("axis0.controller.config.vel_integrator_gain", vel_integ_gain);
    odrive.setParameter("axis1.controller.config.vel_integrator_gain", vel_integ_gain);


    odrive.setParameter("axis0.motor.config.current_lim", 12.0);
    odrive.setParameter("axis1.motor.config.current_lim", 12.0);
    odrive.setParameter("axis0.motor.config.current_lim_margin", 5.0);
    odrive.setParameter("axis1.motor.config.current_lim_margin", 5.0);


    odrive.saveConfig();
    odrive.reboot();
    delay(100);
    // Batterie check
    while(odrive.getParameterAsFloat("vbus_voltage") == 0.0){
      delay(30);
    }
    
    float vBat = odrive.getParameterAsFloat("vbus_voltage");
    if(vBat <= V_BAT_MIN_START) {
      displayManager.vBatLowError(vBat); // Endlosschleife -> Batterie leer
    }
  // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive.getState(0) == AXIS_STATE_UNDEFINED) {
      delay(30);
    }

    errorControl(); //checke nach Errors
    // setze Axis0 in Closed_loop (externen Steuerungsmodus)
    while (odrive.getState(0) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 0);
      delay(10);
    }

      // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive.getState(1) == AXIS_STATE_UNDEFINED) {
      delay(30);
    }

    // setze Axis0 in Closed_loop (externen Steuerungsmodus)
    while (odrive.getState(1) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 1);
      delay(10);
    }

    // aktiviere Watchdog
    //odrive.setParameter("axis0.config.enable_watchdog", true);
    //odrive.setParameter("axis1.config.enable_watchdog", true);



  }   

void idleControle(float odriveKMH, float requestKMH){
  requestKMH = abs(requestKMH);
  if (requestKMH == 0 && odriveKMH == 0){
    if (odrive.getState(0) != AXIS_STATE_IDLE){
      odrive.setState(AXIS_STATE_IDLE, 0);
      odrive.setState(AXIS_STATE_IDLE, 1);
      delay(30);
    }
  }
  else if(requestKMH >= 0.2){
    if (odrive.getState(0) != AXIS_STATE_CLOSED_LOOP_CONTROL){
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 0);
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 1);
      delay(30);
    }
  }

  }

void errorControl(){
  bool errors = true;
  int error = odrive.getParameterAsInt("error");
  int error_0 = odrive.getParameterAsInt("axis0.error");
  int error_1 = odrive.getParameterAsInt("axis1.error");
  while(errors){
    errors = false;
    int error = odrive.getParameterAsInt("error");
    if (error != 0){
      displayManager.displayMessage("ERROR: "+ String(error));
      odrive.setState(AXIS_STATE_IDLE, 0);
      odrive.setState(AXIS_STATE_IDLE, 1);
      errors = true;
    }
    else if (error_0 != 0){
      displayManager.displayMessage("Axis0 ERROR: "+ String(error_0));
      odrive.setState(AXIS_STATE_IDLE, 0);
      odrive.setState(AXIS_STATE_IDLE, 1);
      errors = true;
    }
    else if (error_1 != 0){
      displayManager.displayMessage("Axis1 ERROR: "+ String(error_1));
      odrive.setState(AXIS_STATE_IDLE, 0);
      odrive.setState(AXIS_STATE_IDLE, 1);
      errors = true;
    }
  }
}


void loop() {
  // lies Error und sperre oDrive bei Fehler
  errorControl();

  // leere seriellen Buffer (ggf. unnötig?)
  odrive_serial.flush();
  Serial.flush();

  // setze odrive Watchdog-Timer zurück, wenn dieser nicht alle 0.8 Sekunden zurückgesetzt wird, gehen die Motoren in Notaus (Falls es ein Verbindungsabbruch gibt)
  odrive.resetWatchdog(0); // Setze Watchdog Axis 0 zurück 
  //odrive.resetWatchdog(1); // Setze Watchdog Axis 0 zurück 

  // lese Batteriespannung
  float vBat =  odrive.getParameterAsFloat("vbus_voltage");
  int vBatPercent = map(vBat, V_BAT_MIN, V_BAT_MAX, 0, 100);

  // checke ob Batteriespannung i.O. ist
    if(vBat <= V_BAT_MIN) {
      odrive.setVelocity(0.0); // Bremse auf 0
      displayManager.vBatLowError(vBat); // Endlosschleife -> Batterie leer
    return; // Breche den Loop ab
  }

  // lese aktuelle Geschwindigkeit der angeforderten und tatsächlichen KMH/RPS
  float odriveRPS0 =  odrive.getParameterAsFloat("axis0.encoder.vel_estimate");
  float odriveRPS1 =  odrive.getParameterAsFloat("axis0.encoder.vel_estimate");
 
  float amp = odrive.getParameterAsFloat("axis0.motor.current_control.Iq_measured") + odrive.getParameterAsFloat("axis1.motor.current_control.Iq_measured");
  float nm = amp * odrive.getParameterAsFloat("axis0.motor.config.torque_constant");
  float odriveRPS = (odriveRPS1 + odriveRPS0)/2; // Geschwindigkeitsmittel der zwei Räder
  float odriveKMH = speedController.convertRPStoKMh(odriveRPS);
  float requestRPS = speedController.getRequestedRPS();
  float maxRps = speedController.convertKMHtoRPS(maxKmh);

  float requestKMH = speedController.convertRPStoKMh(requestRPS);


  Serial.print("currentl:");
  Serial.print(odrive.getParameterAsString("axis0.motor.config.current_lim"));
  Serial.print("  1:");
  Serial.print(odrive.getParameterAsString("axis1.motor.config.current_lim_margin"));

  Serial.print("     vg");

  Serial.print(odrive.getParameterAsString("axis1.controller.config.vel_gain"));
  Serial.print("     vig");
  Serial.println(odrive.getParameterAsString("axis1.controller.config.vel_integrator_gain"));

  odrive.setVelocity(requestRPS);

  }


