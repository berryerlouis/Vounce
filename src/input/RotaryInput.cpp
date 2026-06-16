#include "RotaryInput.h"

RotaryInput::RotaryInput(uint8_t pinCLK, uint8_t pinDT)
    : pinCLK(pinCLK), pinDT(pinDT), lastStateCLK(HIGH), lockPin(0), lockEnabled(false) {}

void RotaryInput::begin() {
    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);
    if (lockEnabled) {
        pinMode(lockPin, INPUT_PULLUP);
    }
    lastStateCLK = digitalRead(pinCLK);
}

int RotaryInput::consumeDelta() {
    int delta = 0;
    int stateCLK = digitalRead(pinCLK);

    // Keep decoder phase aligned while the lock button is held.
    if (lockEnabled && digitalRead(lockPin) == LOW) {
        lastStateCLK = stateCLK;
        return 0;
    }

    if (stateCLK != lastStateCLK && stateCLK == HIGH) {
        int stateDT = digitalRead(pinDT);
        delta = (stateDT != stateCLK) ? 1 : -1;
    }

    lastStateCLK = stateCLK;
    return delta;
}

void RotaryInput::setLockWhilePinLow(uint8_t pin) {
    lockPin = pin;
    lockEnabled = true;
}
