#include "ToggleSwitchSensor.h"

namespace {
enum ToggleParamIndex : uint8_t {
    ToggleParamChannel = 0,
    ToggleParamControl,
    ToggleParamValueOff,
    ToggleParamValueOn,
    ToggleParamDebounce,
    ToggleParamCount
};
}

ToggleSwitchSensor::ToggleSwitchSensor(
    uint8_t pin,
    byte channel,
    byte control,
    byte valueOff,
    byte valueOn,
    unsigned long debounceMs)
    : pin(pin),
      channel(channel),
      control(control),
      valueOff(valueOff),
      valueOn(valueOn),
      debounceMs(debounceMs),
      lastReading(HIGH),
      stableState(HIGH),
      lastDebounceTime(0),
      currentValue(valueOff),
      pending(true) {}

void ToggleSwitchSensor::begin() {
    pinMode(pin, INPUT_PULLUP);
    lastReading = digitalRead(pin);
    stableState = lastReading;
}

void ToggleSwitchSensor::update() {
    int reading = digitalRead(pin);

    if (reading != lastReading) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceMs) {
        if (reading != stableState) {
            stableState = reading;

            if (stableState == LOW) {
                currentValue = (currentValue == valueOn) ? valueOff : valueOn;
                pending = true;
            }
        }
    }

    lastReading = reading;
}

bool ToggleSwitchSensor::readMessage(MidiMessage& message) {
    if (!pending) {
        return false;
    }

    message.channel = channel;
    message.control = control;
    message.value = currentValue;

    pending = false;
    return true;
}

const char* ToggleSwitchSensor::getSensorName() const {
    return "ToggleSwitch";
}

uint8_t ToggleSwitchSensor::getParameterCount() const {
    return ToggleParamCount;
}

bool ToggleSwitchSensor::getParameterMeta(uint8_t index, SensorParameterMeta& meta) const {
    switch (index) {
        case ToggleParamChannel:
            meta = {"midi_channel", 0, 15, 1};
            return true;
        case ToggleParamControl:
            meta = {"midi_control", 0, 127, 1};
            return true;
        case ToggleParamValueOff:
            meta = {"value_off", 0, 127, 1};
            return true;
        case ToggleParamValueOn:
            meta = {"value_on", 0, 127, 1};
            return true;
        case ToggleParamDebounce:
            meta = {"debounce_ms", 5, 500, 5};
            return true;
        default:
            return false;
    }
}

int ToggleSwitchSensor::getParameterValue(uint8_t index) const {
    switch (index) {
        case ToggleParamChannel:
            return channel;
        case ToggleParamControl:
            return control;
        case ToggleParamValueOff:
            return valueOff;
        case ToggleParamValueOn:
            return valueOn;
        case ToggleParamDebounce:
            return (int)debounceMs;
        default:
            return 0;
    }
}

void ToggleSwitchSensor::setParameterValue(uint8_t index, int newValue) {
    switch (index) {
        case ToggleParamChannel:
            channel = (byte)constrain(newValue, 0, 15);
            break;
        case ToggleParamControl:
            control = (byte)constrain(newValue, 0, 127);
            break;
        case ToggleParamValueOff:
            valueOff = (byte)constrain(newValue, 0, 127);
            break;
        case ToggleParamValueOn:
            valueOn = (byte)constrain(newValue, 0, 127);
            break;
        case ToggleParamDebounce:
            debounceMs = (unsigned long)constrain(newValue, 5, 500);
            break;
        default:
            break;
    }
}
