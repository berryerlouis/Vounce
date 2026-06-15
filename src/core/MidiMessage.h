#pragma once
#include <Arduino.h>

struct MidiMessage {
    byte channel;
    byte control;
    byte value;
};
