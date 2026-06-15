#pragma once
#include <Arduino.h>
#include "../core/MidiMessage.h"

class MidiOut {
  public:
    explicit MidiOut(HardwareSerial& serial);

    void begin(unsigned long baudRate = 115200);
    void sendCC(byte channel, byte control, byte value);
    void sendCC(const MidiMessage& message);

  private:
    HardwareSerial& serial;
};
