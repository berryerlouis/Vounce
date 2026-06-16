#include "Led.h"

Led::Led(uint8_t pin)
    : pin(pin),
      mode(Off),
      brightness(255),
      blinkPeriod(500),
      lastBlinkTime(0),
      blinkState(false) {}

void Led::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Led::update() {
    if (mode == Off) {
        digitalWrite(pin, LOW);
    } else if (mode == On) {
        applyBrightness();
    } else if (mode == Blink) {
        unsigned long now = millis();
        if (now - lastBlinkTime >= blinkPeriod / 2) {
            blinkState = !blinkState;
            lastBlinkTime = now;
        }

        if (blinkState) {
            applyBrightness();
        } else {
            digitalWrite(pin, LOW);
        }
    }
}

void Led::setMode(Mode newMode) {
    if (mode == newMode) {
        return;
    }

    mode = newMode;
    if (mode == Blink) {
        blinkState = false;
        lastBlinkTime = millis();
    }
}

void Led::setBrightness(uint8_t value) {
    brightness = value;
}

void Led::setBlinkPeriod(unsigned long period) {
    blinkPeriod = period;
}

bool Led::isOn() const {
    return mode != Off;
}

Led::Mode Led::getMode() const {
    return mode;
}

void Led::applyBrightness() {
    if (brightness == 255) {
        digitalWrite(pin, HIGH);
    } else {
        analogWrite(pin, brightness);
    }
}
