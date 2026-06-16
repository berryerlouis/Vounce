#include "MidiOut.h"
#include "../conf/pinout.h"

#ifdef VOUNCE_USE_MIDIUSB
#include <MIDIUSB.h>
#endif

MidiOut::MidiOut(Stream* serial, bool logEnabled)
    : serial(serial), logEnabled(logEnabled) {}

void MidiOut::begin(unsigned long baudRate) {
#ifdef VOUNCE_USE_MIDIUSB
    Serial.begin(baudRate);
    // USB MIDI initializes automatically, no serial needed
#else
    if (serial != nullptr) {
        serial->begin(baudRate);
    }
#endif
}

void MidiOut::sendCC(byte channel, byte control, byte value) {
    serial->println("Channel: " + String(channel) + ", Control: " + String(control) + ", Value: " + String(value));

#ifdef VOUNCE_USE_MIDIUSB
    midiEventPacket_t event = {
        0x0B,
        (uint8_t)(0xB0 | (channel)),
        (uint8_t)(control),
        (uint8_t)(value)
    };
    MidiUSB.sendMIDI(event);
    MidiUSB.flush();

#else
    if (serial != nullptr) {
        serial->write(0xB0 | (channel & 0x0F));
        serial->write(control & 0x7F);
        serial->write(value & 0x7F);
    }
#endif
}

void MidiOut::sendCC(const MidiMessage& message) {
    sendCC(message.channel, message.control, message.value);
}
