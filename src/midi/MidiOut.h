#pragma once
#include <Arduino.h>
#include "../core/MidiMessage.h"

class MidiOut {
  public:
    explicit MidiOut(Stream* serial = nullptr, bool logEnabled = true);

    void begin(unsigned long baudRate = 115200);
    void sendCC(byte channel, byte control, byte value);
    void sendCC(const MidiMessage& message);

  private:
    Stream* serial;
    bool logEnabled;
};

