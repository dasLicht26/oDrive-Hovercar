#ifndef HallSensor_h
#define HallSensor_h


class HallSensor {
  public:
    void setup(int _maxValue, int _minValue) {
      maxValue = _maxValue;
      minValue = _minValue;
    }

    int readValue(int pin) {
      int rawValue = analogRead(pin);
      int toReturn = map(rawValue, 540, 3000, minValue, maxValue); // 0.5V und 3.0V auf 0-100 Skala mappen

      // verhinden, dass Werte unter oder über min/max zurückgegeben werden
      if(toReturn <= minValue){
        toReturn = minValue;
      }
      if(toReturn >= maxValue){
        toReturn = maxValue;
      }

      return toReturn; 
    }

  private:
    int maxValue;
    int minValue;
};

#endif