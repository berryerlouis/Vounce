#pragma once
#include <Arduino.h>

class Button {
  public:
      Button(uint8_t pin, unsigned long longPressTime = 1000, unsigned long debounce = 30);

      void begin();
      bool status(void);
      void onShortPress(void (*cb)());
      void onLongPress(void (*cb)());
      void update();

  private:
      uint8_t pin;
      unsigned long longPressTime;
      unsigned long debounce;

      int lastState = HIGH;
      int lastReading = HIGH;

      bool pressed = false;
      unsigned long pressStart = 0;
      unsigned long lastDebounceTime = 0;

      void (*shortPressCallback)() = nullptr;
      void (*longPressCallback)() = nullptr;
};
