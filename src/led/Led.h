#pragma once
#include <Arduino.h>

class Led {
  public:
    enum Mode : uint8_t {
        Off = 0,
        On = 1,
        Blink = 2
    };

    Led(uint8_t pin);

    void begin();
    void update();

    void setMode(Mode mode);
    void setBrightness(uint8_t brightness);
    void blink(unsigned long period);

    bool isOn() const;
    Mode getMode() const;

  private:
    uint8_t pin;
    Mode mode;
    uint8_t brightness;
    unsigned long blinkInterval;
    unsigned long lastBlinkTime;
    unsigned long blinkEndPeriod;
    bool blinkState;

    void applyBrightness();
};
