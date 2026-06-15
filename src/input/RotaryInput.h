#pragma once
#include <Arduino.h>

class RotaryInput {
  public:
    RotaryInput(uint8_t pinCLK, uint8_t pinDT);

    void begin();
    int consumeDelta();

  private:
    uint8_t pinCLK;
    uint8_t pinDT;
    int lastStateCLK;
};
