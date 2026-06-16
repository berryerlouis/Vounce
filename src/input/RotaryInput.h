#pragma once
#include <Arduino.h>

class RotaryInput {
  public:
    RotaryInput(uint8_t pinCLK, uint8_t pinDT);

    void begin();
    int consumeDelta();
    void setLockWhilePinLow(uint8_t pin);

  private:
    uint8_t pinCLK;
    uint8_t pinDT;
    int lastStateCLK;
    uint8_t lockPin;
    bool lockEnabled;
};
