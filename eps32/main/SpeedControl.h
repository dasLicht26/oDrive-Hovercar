#ifndef SpeedControler_h
#define SpeedControler_h


class SpeedControler {
  public:
    void setup(int _maxValue, int _minValue) {
      maxValue = _maxValue;
      minValue = _minValue;
    }

    int getSpeed(int forwartValue, int backwardValue, int currentSpeed) {
      int speedValue = forwartValue - backwardValue;
      return speedValue; 
    }

  private:
    int maxValue;
    int minValue;
};

#endif