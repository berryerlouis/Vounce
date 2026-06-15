#include "MidiOut.h"

MidiOut::MidiOut(HardwareSerial& serial)
    : serial(serial) {}

void MidiOut::begin(unsigned long baudRate) {
    serial.begin(baudRate);
}

void MidiOut::sendCC(byte channel, byte control, byte value) {
    serial.write(0xB0 | (channel & 0x0F));
    serial.write(control & 0x7F);
    serial.write(value & 0x7F);
}

void MidiOut::sendCC(const MidiMessage& message) {
    sendCC(message.channel, message.control, message.value);
}
