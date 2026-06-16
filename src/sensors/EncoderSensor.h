#pragma once
#include <Arduino.h>
#include "../core/IMidiSensor.h"
#include "../core/IConfigurableSensor.h"

class EncoderSensor : public IMidiSensor, public IConfigurableSensor {
  public:
    EncoderSensor(
      uint8_t pinCLK,
      uint8_t pinDT,
      byte channel,
      byte control,
      int initialValue = 64,
      byte sensitivity = 1,
      byte minValue = 0,
      byte maxValue = 127
    );

    void begin() override;
    void update() override;
    bool readMessage(MidiMessage& message) override;

    const char* getSensorName() const override;
    uint8_t getParameterCount() const override;
    bool getParameterMeta(uint8_t index, SensorParameterMeta& meta) const override;
    int getParameterValue(uint8_t index) const override;
    void setParameterValue(uint8_t index, int value) override;

    void setSensitivity(byte newSensitivity);
    void setRange(byte minValue, byte maxValue);
    void setLockWhilePinLow(uint8_t pin);

  private:
    uint8_t pinCLK;
    uint8_t pinDT;

    byte channel;
    byte control;
    byte sensitivity;
    byte minValue;
    byte maxValue;

    int value;
    int lastSentValue;

    int lastStateCLK;
    bool pending;
    uint8_t lockPin;
    bool lockEnabled;
};
