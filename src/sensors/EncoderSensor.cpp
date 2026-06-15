#include "EncoderSensor.h"

namespace {
enum EncoderParamIndex : uint8_t {
    EncoderParamChannel = 0,
    EncoderParamControl,
    EncoderParamMin,
    EncoderParamMax,
    EncoderParamSensitivity,
    EncoderParamCount
};
}

EncoderSensor::EncoderSensor(
    uint8_t pinCLK,
    uint8_t pinDT,
    byte channel,
    byte control,
    int initialValue,
    byte sensitivity,
    byte minValue,
    byte maxValue)
    : pinCLK(pinCLK),
      pinDT(pinDT),
      channel(channel),
      control(control),
      sensitivity(sensitivity == 0 ? 1 : sensitivity),
      minValue(minValue),
      maxValue(maxValue),
      value(constrain(initialValue, minValue, maxValue)),
      lastSentValue(-1),
      lastStateCLK(HIGH),
      pending(true) {}

void EncoderSensor::begin() {
    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);
    lastStateCLK = digitalRead(pinCLK);
}

void EncoderSensor::update() {
    int stateCLK = digitalRead(pinCLK);
    int stateDT = digitalRead(pinDT);

    if (stateCLK != lastStateCLK && stateCLK == HIGH) {
        if (stateDT != stateCLK) {
            value += sensitivity;
        } else {
            value -= sensitivity;
        }

        value = constrain(value, minValue, maxValue);

        if (value != lastSentValue) {
            pending = true;
        }
    }

    lastStateCLK = stateCLK;
}

bool EncoderSensor::readMessage(MidiMessage& message) {
    if (!pending) {
        return false;
    }

    message.channel = channel;
    message.control = control;
    message.value = (byte)value;

    lastSentValue = value;
    pending = false;
    return true;
}

const char* EncoderSensor::getSensorName() const {
    return "Encoder";
}

uint8_t EncoderSensor::getParameterCount() const {
    return EncoderParamCount;
}

bool EncoderSensor::getParameterMeta(uint8_t index, SensorParameterMeta& meta) const {
    switch (index) {
        case EncoderParamChannel:
            meta = {"midi_channel", 0, 15, 1};
            return true;
        case EncoderParamControl:
            meta = {"midi_control", 0, 127, 1};
            return true;
        case EncoderParamMin:
            meta = {"min", 0, 127, 1};
            return true;
        case EncoderParamMax:
            meta = {"max", 0, 127, 1};
            return true;
        case EncoderParamSensitivity:
            meta = {"sensitivity", 1, 16, 1};
            return true;
        default:
            return false;
    }
}

int EncoderSensor::getParameterValue(uint8_t index) const {
    switch (index) {
        case EncoderParamChannel:
            return channel;
        case EncoderParamControl:
            return control;
        case EncoderParamMin:
            return minValue;
        case EncoderParamMax:
            return maxValue;
        case EncoderParamSensitivity:
            return sensitivity;
        default:
            return 0;
    }
}

void EncoderSensor::setParameterValue(uint8_t index, int newValue) {
    switch (index) {
        case EncoderParamChannel:
            channel = (byte)constrain(newValue, 0, 15);
            break;
        case EncoderParamControl:
            control = (byte)constrain(newValue, 0, 127);
            break;
        case EncoderParamMin:
            minValue = (byte)constrain(newValue, 0, 127);
            if (minValue > maxValue) {
                maxValue = minValue;
            }
            value = constrain(value, minValue, maxValue);
            break;
        case EncoderParamMax:
            maxValue = (byte)constrain(newValue, 0, 127);
            if (maxValue < minValue) {
                minValue = maxValue;
            }
            value = constrain(value, minValue, maxValue);
            break;
        case EncoderParamSensitivity:
            sensitivity = (byte)constrain(newValue, 1, 16);
            break;
        default:
            break;
    }
}

void EncoderSensor::setSensitivity(byte newSensitivity) {
    sensitivity = (newSensitivity == 0) ? 1 : newSensitivity;
}

void EncoderSensor::setRange(byte newMinValue, byte newMaxValue) {
    minValue = newMinValue;
    maxValue = newMaxValue;
    value = constrain(value, minValue, maxValue);
}
