#include "Button.h"

Button::Button(uint8_t pin, unsigned long longPressTime, unsigned long debounce)
    : pin(pin), longPressTime(longPressTime), debounce(debounce) {}

void Button::begin() {
    pinMode(pin, INPUT_PULLUP);
    lastState = digitalRead(pin);
}


bool Button::status(void) {
    return pressed;
}

void Button::onShortPress(void (*cb)()) {
    shortPressCallback = cb;
}

void Button::onLongPress(void (*cb)()) {
    longPressCallback = cb;
}

void Button::update() {
    int reading = digitalRead(pin);
    unsigned long now = millis();

    // Anti-rebond
    if (reading != lastReading) {
        lastDebounceTime = now;
    }
    lastReading = reading;

    if ((now - lastDebounceTime) > debounce) {

        // Bouton pressé
        if (reading == LOW && !pressed) {
            pressed = true;
            pressStart = now;
        }

        // Bouton relâché
        if (reading == HIGH && pressed) {
            pressed = false;

            unsigned long pressDuration = now - pressStart;

            if (pressDuration >= longPressTime) {
                if (longPressCallback) longPressCallback();
            } else {
                if (shortPressCallback) shortPressCallback();
            }
        }
    }

    lastState = reading;
}
