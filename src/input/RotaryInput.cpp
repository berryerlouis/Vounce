#include "RotaryInput.h"

RotaryInput::RotaryInput(uint8_t pinCLK, uint8_t pinDT)
    : pinCLK(pinCLK), pinDT(pinDT), lastStateCLK(HIGH) {}

void RotaryInput::begin() {
    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);
    lastStateCLK = digitalRead(pinCLK);
}

int RotaryInput::consumeDelta() {
    int delta = 0;
    int stateCLK = digitalRead(pinCLK);

    if (stateCLK != lastStateCLK && stateCLK == HIGH) {
        int stateDT = digitalRead(pinDT);
        delta = (stateDT != stateCLK) ? 1 : -1;
    }

    lastStateCLK = stateCLK;
    return delta;
}
