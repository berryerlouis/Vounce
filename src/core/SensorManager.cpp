#include "SensorManager.h"

SensorManager::SensorManager()
    : sensors{nullptr}, sensorCount(0), enabled(true) {}

bool SensorManager::addSensor(IMidiSensor& sensor) {
    if (sensorCount >= MaxSensors) {
        return false;
    }

    sensors[sensorCount] = &sensor;
    sensorCount++;
    return true;
}

void SensorManager::begin() {
    for (uint8_t i = 0; i < sensorCount; i++) {
        sensors[i]->begin();
    }
}

void SensorManager::setEnabled(bool isEnabled) {
    enabled = isEnabled;
}

void SensorManager::updateAndSend(MidiOut& midiOut) {
    if (!enabled) {
        return;
    }

    MidiMessage message;
    bool sentFromSensorThisTick = false;

    for (uint8_t i = 0; i < sensorCount; i++) {
        sensors[i]->update();

        while (sensors[i]->readMessage(message)) {
            midiOut.sendCC(message);
            sentFromSensorThisTick = true;
        }

        // Emit messages from one sensor per loop tick to avoid mixed bursts
        // when multiple sensors change at nearly the same time.
        if (sentFromSensorThisTick) {
            break;
        }
    }
}
