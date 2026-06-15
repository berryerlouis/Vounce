#pragma once
#include <Arduino.h>
#include "../core/IMidiSensor.h"
#include "../core/IConfigurableSensor.h"

class UltrasonicSensor : public IMidiSensor, public IConfigurableSensor {
  public:
    UltrasonicSensor(
      uint8_t trigPin,
      uint8_t echoPin,
      byte channel,
      byte control,
      int minDistanceCm = 5,
      int maxDistanceCm = 80,
      byte midiMin = 0,
      byte midiMax = 127,
      unsigned long sampleIntervalMs = 50,
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
    uint8_t trigPin;
    uint8_t echoPin;
    byte channel;
    byte control;

    int minDistanceCm;
    int maxDistanceCm;
    byte midiMin;
    byte midiMax;
    unsigned long sampleIntervalMs;
    byte changeThreshold;

    unsigned long lastSampleMs;
    int currentValue;
    int lastSentValue;
    bool pending;
};
