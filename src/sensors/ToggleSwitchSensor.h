#pragma once
#include <Arduino.h>
#include "../core/IMidiSensor.h"
#include "../core/IConfigurableSensor.h"

class ToggleSwitchSensor : public IMidiSensor, public IConfigurableSensor {
  public:
    ToggleSwitchSensor(
      uint8_t pin,
      byte channel,
      byte control,
      byte valueOff = 0,
      byte valueOn = 127,
      unsigned long debounceMs = 30
    );

    void begin() override;
    void update() override;
    bool readMessage(MidiMessage& message) override;

    const char* getSensorName() const override;
    uint8_t getParameterCount() const override;
    bool getParameterMeta(uint8_t index, SensorParameterMeta& meta) const override;
    int getParameterValue(uint8_t index) const override;
    void setParameterValue(uint8_t index, int value) override;

  private:
    uint8_t pin;
    byte channel;
    byte control;
    byte valueOff;
    byte valueOn;
    unsigned long debounceMs;

    int lastReading;
    int stableState;
    unsigned long lastDebounceTime;

    byte currentValue;
    bool pending;
    bool pressInProgress;
    unsigned long pressStartMs;

    static const unsigned long MinValidPressMs = 60;
};
