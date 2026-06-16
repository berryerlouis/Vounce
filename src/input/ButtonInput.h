#pragma once
#include <Arduino.h>

class ButtonInput {
  public:
    ButtonInput(uint8_t pin, unsigned long debounceMs = 30, unsigned long longPressMs = 800);

    void begin();
    void update();

    bool consumeShortPress();
    bool consumeLongPress();
    bool isPressed() const;

  private:
    uint8_t pin;
    unsigned long debounceMs;
    unsigned long longPressMs;

    int lastReading;
    int stableState;
    unsigned long lastDebounceTime;

    bool pressed;
    bool longPressTriggered;
    unsigned long pressStartMs;

    bool shortPressPending;
    bool longPressPending;
};
