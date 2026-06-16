#include "ButtonInput.h"

ButtonInput::ButtonInput(uint8_t pin, unsigned long debounceMs, unsigned long longPressMs)
    : pin(pin),
      debounceMs(debounceMs),
      longPressMs(longPressMs),
      lastReading(HIGH),
      stableState(HIGH),
      lastDebounceTime(0),
      pressed(false),
      longPressTriggered(false),
      pressStartMs(0),
      shortPressPending(false),
      longPressPending(false) {}

void ButtonInput::begin() {
    pinMode(pin, INPUT_PULLUP);
    lastReading = digitalRead(pin);
    stableState = lastReading;
}

void ButtonInput::update() {
    int reading = digitalRead(pin);
    unsigned long now = millis();

    if (reading != lastReading) {
        lastDebounceTime = now;
    }

    if ((now - lastDebounceTime) > debounceMs) {
        if (reading != stableState) {
            stableState = reading;

            if (stableState == LOW) {
                pressed = true;
                longPressTriggered = false;
                pressStartMs = now;
            } else {
                if (pressed && !longPressTriggered) {
                    shortPressPending = true;
                }
                pressed = false;
            }
        }
    }

    if (pressed && !longPressTriggered && (now - pressStartMs >= longPressMs)) {
        longPressTriggered = true;
        longPressPending = true;
    }

    lastReading = reading;
}

bool ButtonInput::consumeShortPress() {
    bool out = shortPressPending;
    shortPressPending = false;
    return out;
}

bool ButtonInput::consumeLongPress() {
    bool out = longPressPending;
    longPressPending = false;
    return out;
}

bool ButtonInput::isPressed() const {
    return pressed;
}
