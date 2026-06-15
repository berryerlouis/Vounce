#pragma once
#include <Arduino.h>
#include "MidiMessage.h"

class IMidiSensor {
  public:
    virtual ~IMidiSensor() = default;

    virtual void begin() = 0;
    virtual void update() = 0;

    // Returns true when a fresh MIDI message is available.
    virtual bool readMessage(MidiMessage& message) = 0;
};
