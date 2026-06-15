#include "PotentiometerSensor.h"

namespace {
enum PotParamIndex : uint8_t {
    PotParamChannel = 0,
    PotParamControl,
    PotParamAnalogMin,
    PotParamAnalogMax,
    PotParamMidiMin,
    PotParamMidiMax,
    PotParamThreshold,
    PotParamCount
};
}

PotentiometerSensor::PotentiometerSensor(
    uint8_t analogPin,
    byte channel,
    byte control,
    int analogMin,
    int analogMax,
    byte midiMin,
    byte midiMax,
    byte changeThreshold)
    : analogPin(analogPin),
      channel(channel),
      control(control),
      analogMin(analogMin),
      analogMax(analogMax),
      midiMin(midiMin),
      midiMax(midiMax),
      changeThreshold(changeThreshold == 0 ? 1 : changeThreshold),
      currentValue(midiMin),
      lastSentValue(-1),
      pending(true) {}

void PotentiometerSensor::begin() {
    pinMode(analogPin, INPUT);
}

void PotentiometerSensor::update() {
    int raw = analogRead(analogPin);
    raw = constrain(raw, analogMin, analogMax);

    int mapped = map(raw, analogMin, analogMax, midiMin, midiMax);
    mapped = constrain(mapped, midiMin, midiMax);
    currentValue = mapped;

    if (abs(currentValue - lastSentValue) >= changeThreshold) {
        pending = true;
    }
}

bool PotentiometerSensor::readMessage(MidiMessage& message) {
    if (!pending) {
        return false;
    }

    message.channel = channel;
    message.control = control;
    message.value = (byte)currentValue;

    lastSentValue = currentValue;
    pending = false;
    return true;
}

const char* PotentiometerSensor::getSensorName() const {
    return "Potentiometer";
}

uint8_t PotentiometerSensor::getParameterCount() const {
    return PotParamCount;
}

bool PotentiometerSensor::getParameterMeta(uint8_t index, SensorParameterMeta& meta) const {
    switch (index) {
        case PotParamChannel:
            meta = {"midi_channel", 0, 15, 1};
            return true;
        case PotParamControl:
            meta = {"midi_control", 0, 127, 1};
            return true;
        case PotParamAnalogMin:
            meta = {"analog_min", 0, 1023, 1};
            return true;
        case PotParamAnalogMax:
            meta = {"analog_max", 0, 1023, 1};
            return true;
        case PotParamMidiMin:
            meta = {"midi_min", 0, 127, 1};
            return true;
        case PotParamMidiMax:
            meta = {"midi_max", 0, 127, 1};
            return true;
        case PotParamThreshold:
            meta = {"threshold", 1, 16, 1};
            return true;
        default:
            return false;
    }
}

int PotentiometerSensor::getParameterValue(uint8_t index) const {
    switch (index) {
        case PotParamChannel:
            return channel;
        case PotParamControl:
            return control;
        case PotParamAnalogMin:
            return analogMin;
        case PotParamAnalogMax:
            return analogMax;
        case PotParamMidiMin:
            return midiMin;
        case PotParamMidiMax:
            return midiMax;
        case PotParamThreshold:
            return changeThreshold;
        default:
            return 0;
    }
}

void PotentiometerSensor::setParameterValue(uint8_t index, int newValue) {
    switch (index) {
        case PotParamChannel:
            channel = (byte)constrain(newValue, 0, 15);
            break;
        case PotParamControl:
            control = (byte)constrain(newValue, 0, 127);
            break;
        case PotParamAnalogMin:
            analogMin = constrain(newValue, 0, 1023);
            if (analogMin > analogMax) {
                analogMax = analogMin;
            }
            break;
        case PotParamAnalogMax:
            analogMax = constrain(newValue, 0, 1023);
            if (analogMax < analogMin) {
                analogMin = analogMax;
            }
            break;
        case PotParamMidiMin:
            midiMin = (byte)constrain(newValue, 0, 127);
            if (midiMin > midiMax) {
                midiMax = midiMin;
            }
            break;
        case PotParamMidiMax:
            midiMax = (byte)constrain(newValue, 0, 127);
            if (midiMax < midiMin) {
                midiMin = midiMax;
            }
            break;
        case PotParamThreshold:
            changeThreshold = (byte)constrain(newValue, 1, 16);
            break;
        default:
            break;
    }
}
