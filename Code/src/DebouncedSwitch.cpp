#include <Arduino.h>

#define DEBOUNCE_TIME 10


class DebouncedSwitch {
  private:
    int pin;

    bool curState;
    bool targetState;

    unsigned long stateChangeTime;
    
    void (*onChange)(bool);

  public:

  DebouncedSwitch(int pin, void (*onChange)(bool) = nullptr) : onChange(onChange), curState(0), targetState(0), pin(pin), stateChangeTime(0) {
    pinMode(pin, INPUT);
  }

  void run() {
    if (digitalRead(pin) != targetState) {
      //State changed
      stateChangeTime = millis();
      targetState = !targetState;     
    } else if (stateChangeTime > 0 && millis() > stateChangeTime + DEBOUNCE_TIME) {
      curState = targetState;

      if (onChange != nullptr) {
        (*onChange)(curState);
      }

      stateChangeTime = 0;
    }
  }

  bool getState() {
    return curState;
  }
};