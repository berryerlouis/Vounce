#include "UltrasonicSensor.h"

namespace {
enum UltraParamIndex : uint8_t {
    UltraParamChannel = 0,
    UltraParamControl,
    UltraParamMinDistance,
    UltraParamMaxDistance,
    UltraParamMidiMin,
    UltraParamMidiMax,
    UltraParamSampleMs,
    UltraParamThreshold,
    UltraParamCount
};
}

UltrasonicSensor::UltrasonicSensor(
    uint8_t trigPin,
    uint8_t echoPin,
    byte channel,
    byte control,
    int minDistanceCm,
    int maxDistanceCm,
    byte midiMin,
    byte midiMax,
    unsigned long sampleIntervalMs,
    byte changeThreshold)
    : trigPin(trigPin),
      echoPin(echoPin),
      channel(channel),
      control(control),
      minDistanceCm(minDistanceCm),
      maxDistanceCm(maxDistanceCm),
      midiMin(midiMin),
      midiMax(midiMax),
      sampleIntervalMs(sampleIntervalMs),
      changeThreshold(changeThreshold == 0 ? 1 : changeThreshold),
      lastSampleMs(0),
      currentValue(midiMin),
      lastSentValue(-1),
      pending(true) {}

void UltrasonicSensor::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

void UltrasonicSensor::update() {
    unsigned long now = millis();
    if (now - lastSampleMs < sampleIntervalMs) {
        return;
    }
    lastSampleMs = now;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    unsigned long durationUs = pulseIn(echoPin, HIGH, 30000UL);
    if (durationUs == 0) {
        return;
    }

    int distanceCm = (int)(durationUs / 58UL);
    distanceCm = constrain(distanceCm, minDistanceCm, maxDistanceCm);

    int mapped = map(distanceCm, minDistanceCm, maxDistanceCm, midiMin, midiMax);
    mapped = constrain(mapped, midiMin, midiMax);
    currentValue = mapped;

    if (abs(currentValue - lastSentValue) >= changeThreshold) {
        pending = true;
    }
}

bool UltrasonicSensor::readMessage(MidiMessage& message) {
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

const char* UltrasonicSensor::getSensorName() const {
    return "Ultrasonic";
}

uint8_t UltrasonicSensor::getParameterCount() const {
    return UltraParamCount;
}

bool UltrasonicSensor::getParameterMeta(uint8_t index, SensorParameterMeta& meta) const {
    switch (index) {
        case UltraParamChannel:
            meta = {"midi_channel", 0, 15, 1};
            return true;
        case UltraParamControl:
            meta = {"midi_control", 0, 127, 1};
            return true;
        case UltraParamMinDistance:
            meta = {"dist_min_cm", 1, 400, 1};
            return true;
        case UltraParamMaxDistance:
            meta = {"dist_max_cm", 1, 400, 1};
            return true;
        case UltraParamMidiMin:
            meta = {"midi_min", 0, 127, 1};
            return true;
        case UltraParamMidiMax:
            meta = {"midi_max", 0, 127, 1};
            return true;
        case UltraParamSampleMs:
            meta = {"sample_ms", 10, 500, 5};
            return true;
        case UltraParamThreshold:
            meta = {"threshold", 1, 16, 1};
            return true;
        default:
            return false;
    }
}

int UltrasonicSensor::getParameterValue(uint8_t index) const {
    switch (index) {
        case UltraParamChannel:
            return channel;
        case UltraParamControl:
            return control;
        case UltraParamMinDistance:
            return minDistanceCm;
        case UltraParamMaxDistance:
            return maxDistanceCm;
        case UltraParamMidiMin:
            return midiMin;
        case UltraParamMidiMax:
            return midiMax;
        case UltraParamSampleMs:
            return (int)sampleIntervalMs;
        case UltraParamThreshold:
            return changeThreshold;
        default:
            return 0;
    }
}

void UltrasonicSensor::setParameterValue(uint8_t index, int newValue) {
    switch (index) {
        case UltraParamChannel:
            channel = (byte)constrain(newValue, 0, 15);
            break;
        case UltraParamControl:
            control = (byte)constrain(newValue, 0, 127);
            break;
        case UltraParamMinDistance:
            minDistanceCm = constrain(newValue, 1, 400);
            if (minDistanceCm > maxDistanceCm) {
                maxDistanceCm = minDistanceCm;
            }
            break;
        case UltraParamMaxDistance:
            maxDistanceCm = constrain(newValue, 1, 400);
            if (maxDistanceCm < minDistanceCm) {
                minDistanceCm = maxDistanceCm;
            }
            break;
        case UltraParamMidiMin:
            midiMin = (byte)constrain(newValue, 0, 127);
            if (midiMin > midiMax) {
                midiMax = midiMin;
            }
            break;
        case UltraParamMidiMax:
            midiMax = (byte)constrain(newValue, 0, 127);
            if (midiMax < midiMin) {
                midiMin = midiMax;
            }
            break;
        case UltraParamSampleMs:
            sampleIntervalMs = (unsigned long)constrain(newValue, 10, 500);
            break;
        case UltraParamThreshold:
            changeThreshold = (byte)constrain(newValue, 1, 16);
            break;
        default:
            break;
    }
}
