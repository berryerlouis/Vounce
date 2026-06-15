#pragma once
#include <Arduino.h>
#include "IMidiSensor.h"
#include "MidiMessage.h"
#include "../midi/MidiOut.h"

class SensorManager {
  public:
    static const uint8_t MaxSensors = 8;

    SensorManager();

    bool addSensor(IMidiSensor& sensor);
    void begin();
    void setEnabled(bool enabled);
    void updateAndSend(MidiOut& midiOut);

  private:
    IMidiSensor* sensors[MaxSensors];
    uint8_t sensorCount;
    bool enabled;
};
