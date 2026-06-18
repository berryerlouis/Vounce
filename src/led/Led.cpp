#include "Led.h"

Led::Led(uint8_t pin)
    : pin(pin),
      mode(Off),
      brightness(255),
      blinkInterval(500),
      lastBlinkTime(0),
      blinkEndPeriod(0),
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
        if (millis() >= blinkEndPeriod) {
            digitalWrite(pin, LOW);
            mode = Off;
        } else {
            unsigned long now = millis();
            if (now - lastBlinkTime >= blinkInterval / 2) {
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

void Led::blink(unsigned long period) {
    unsigned long now = millis();
    mode = Blink;
    blinkInterval = 100;
    if (period > 0 && period < blinkInterval) {
        blinkInterval = period;
    }

    // Restart the pulse every time and turn ON immediately for clear feedback.
    blinkState = true;
    lastBlinkTime = now;
    blinkEndPeriod = now + period;
    applyBrightness();
}


void Led::setBrightness(uint8_t value) {
    brightness = value;
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
