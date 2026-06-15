#pragma once
#include <Arduino.h>
#include "../core/IMidiSensor.h"
#include "../core/IConfigurableSensor.h"

class PotentiometerSensor : public IMidiSensor, public IConfigurableSensor {
  public:
    PotentiometerSensor(
      uint8_t analogPin,
      byte channel,
      byte control,
      int analogMin = 0,
      int analogMax = 1023,
      byte midiMin = 0,
      byte midiMax = 127,
      byte changeThreshold = 1
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
    uint8_t analogPin;
    byte channel;
    byte control;

    int analogMin;
    int analogMax;
    byte midiMin;
    byte midiMax;
    byte changeThreshold;

    int currentValue;
    int lastSentValue;
    bool pending;
};
