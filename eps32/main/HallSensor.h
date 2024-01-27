#ifndef HallSensor_h
#define HallSensor_h

class HallSensor {
  public:
    void setup() {
      // Initialisierung falls benötigt
    }

    int readValue(int pin) {
      int rawValue = analogRead(pin);
      return map(rawValue, 112, 4095, 0, 100); // 1.1V und 3.2V auf 0-100 Skala
    }
};

#endif