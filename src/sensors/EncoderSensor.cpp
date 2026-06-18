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

constexpr unsigned long LockTransitionSettleMs = 30;
constexpr unsigned long LockDebounceMs = 8*3;
constexpr unsigned long EncoderTransitionMinUs = 250;

// Transition lookup for quadrature states (old_state << 2 | new_state).
// State bit layout is AB where A=CLK and B=DT.
constexpr int8_t QuadratureTransitionTable[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0
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
            lastABState(0),
            stepAccumulator(0),
    pending(true),
    lockPin(0),
        lockEnabled(false),
        lockRawStateLow(false),
        lockStableStateLow(false),
        lockRawChangeMs(0),
        suppressUntilMs(0),
        lastTransitionUs(0) {}

void EncoderSensor::begin() {
    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);
    if (lockEnabled) {
        pinMode(lockPin, INPUT_PULLUP);
        bool initialLow = (digitalRead(lockPin) == LOW);
        lockRawStateLow = initialLow;
        lockStableStateLow = initialLow;
        lockRawChangeMs = millis();
        suppressUntilMs = 0;
    }
    uint8_t clk = (uint8_t)digitalRead(pinCLK);
    uint8_t dt = (uint8_t)digitalRead(pinDT);
    lastABState = (uint8_t)((clk << 1) | dt);
    stepAccumulator = 0;
    lastTransitionUs = micros();
}

void EncoderSensor::update() {
    uint8_t stateCLK = (uint8_t)digitalRead(pinCLK);
    uint8_t stateDT = (uint8_t)digitalRead(pinDT);
    uint8_t currentABState = (uint8_t)((stateCLK << 1) | stateDT);

    // Ignore encoder movement while the lock pin is active and briefly after
    // any button edge so push/release bounce does not look like rotation.
    if (lockEnabled) {
        bool lockRawLow = (digitalRead(lockPin) == LOW);
        unsigned long now = millis();

        if (lockRawLow != lockRawStateLow) {
            lockRawStateLow = lockRawLow;
            lockRawChangeMs = now;
        }

        if ((now - lockRawChangeMs) >= LockDebounceMs && lockStableStateLow != lockRawStateLow) {
            bool wasPressed = lockStableStateLow;
            lockStableStateLow = lockRawStateLow;

            // When a confirmed release occurs, suppress brief false deltas.
            if (wasPressed && !lockStableStateLow) {
                suppressUntilMs = now + LockTransitionSettleMs;
            }
        }

        if (lockStableStateLow || now < suppressUntilMs) {
            lastABState = currentABState;
            stepAccumulator = 0;
            return;
        }
    }

    if (currentABState != lastABState) {
        unsigned long nowUs = micros();
        if ((unsigned long)(nowUs - lastTransitionUs) < EncoderTransitionMinUs) {
            // Keep phase aligned when ignoring ultra-fast glitches.
            lastABState = currentABState;
            return;
        }
        lastTransitionUs = nowUs;

        uint8_t transitionIndex = (uint8_t)((lastABState << 2) | currentABState);

        // Invert direction to preserve previous project behavior.
        int8_t quarterStep = (int8_t)(-QuadratureTransitionTable[transitionIndex]);
        if (quarterStep == 0) {
            // Invalid/skipped transition: resync phase and discard partial step.
            stepAccumulator = 0;
            lastABState = currentABState;
            return;
        }

        if ((stepAccumulator > 0 && quarterStep < 0) || (stepAccumulator < 0 && quarterStep > 0)) {
            // Direction flipped mid-sequence (often bounce), restart accumulation.
            stepAccumulator = quarterStep;
        } else {
            stepAccumulator = (int8_t)(stepAccumulator + quarterStep);
        }

        if (stepAccumulator >= 2 || stepAccumulator <= -2) {
            int direction = (stepAccumulator > 0) ? 1 : -1;
            stepAccumulator = 0;

            value += direction * sensitivity;
            value = constrain(value, minValue, maxValue);

            if (value != lastSentValue) {
                pending = true;
            }
        }

        lastABState = currentABState;
    }
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
            meta = {"midi_channel", 1, 16, 1};
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
            return channel + 1;
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
            channel = (byte)constrain(newValue, 1, 16) - 1;
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

void EncoderSensor::setLockWhilePinLow(uint8_t pin) {
    lockPin = pin;
    lockEnabled = true;
}
