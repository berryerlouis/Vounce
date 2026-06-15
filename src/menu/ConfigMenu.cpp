#include "ConfigMenu.h"

ConfigMenu::ConfigMenu(ButtonInput& button, RotaryInput& rotary, Stream& output)
    : button(button),
      rotary(rotary),
      output(output),
      sensors{nullptr},
      sensorCount(0),
      mode(Closed),
      selectedSensor(0),
      selectedParam(0) {}

void ConfigMenu::begin() {
    button.begin();
    rotary.begin();
}

bool ConfigMenu::addSensor(IConfigurableSensor& sensor) {
    if (sensorCount >= MaxSensors) {
        return false;
    }

    sensors[sensorCount] = &sensor;
    sensorCount++;
    return true;
}

void ConfigMenu::update() {
    button.update();

    if (button.consumeLongPress()) {
        handleLongPress();
    }

    if (button.consumeShortPress()) {
        handleShortPress();
    }

    if (mode != Closed) {
        int delta = rotary.consumeDelta();
        if (delta != 0) {
            handleEncoderDelta(delta);
        }
    }
}

bool ConfigMenu::isOpen() const {
    return mode != Closed;
}

void ConfigMenu::open() {
    if (sensorCount == 0) {
        return;
    }

    mode = SelectSensor;
    selectedSensor = 0;
    selectedParam = 0;
    printState();
}

void ConfigMenu::close() {
    mode = Closed;
    output.println(F("[MENU] closed"));
}

void ConfigMenu::handleShortPress() {
    if (mode == Closed) {
        return;
    }

    if (mode == SelectSensor) {
        selectedParam = 0;
        mode = SelectParam;
        printState();
        return;
    }

    if (mode == SelectParam) {
        mode = EditParam;
        printState();
        return;
    }

    if (mode == EditParam) {
        mode = SelectParam;
        printState();
    }
}

void ConfigMenu::handleLongPress() {
    if (mode == Closed) {
        open();
        return;
    }

    if (mode == EditParam) {
        mode = SelectParam;
        printState();
        return;
    }

    if (mode == SelectParam) {
        mode = SelectSensor;
        printState();
        return;
    }

    close();
}

void ConfigMenu::handleEncoderDelta(int delta) {
    if (sensorCount == 0) {
        return;
    }

    if (mode == SelectSensor) {
        int next = (int)selectedSensor + delta;
        if (next < 0) {
            next = sensorCount - 1;
        }
        if (next >= sensorCount) {
            next = 0;
        }
        selectedSensor = (uint8_t)next;
        selectedParam = 0;
        printState();
        return;
    }

    IConfigurableSensor* sensor = sensors[selectedSensor];
    uint8_t parameterCount = sensor->getParameterCount();
    if (parameterCount == 0) {
        return;
    }

    if (mode == SelectParam) {
        int next = (int)selectedParam + delta;
        if (next < 0) {
            next = parameterCount - 1;
        }
        if (next >= parameterCount) {
            next = 0;
        }
        selectedParam = (uint8_t)next;
        printState();
        return;
    }

    if (mode == EditParam) {
        SensorParameterMeta meta;
        if (!sensor->getParameterMeta(selectedParam, meta)) {
            return;
        }

        int current = sensor->getParameterValue(selectedParam);
        int next = current + (delta * meta.step);
        next = constrain(next, meta.minValue, meta.maxValue);
        sensor->setParameterValue(selectedParam, next);
        printState();
    }
}

void ConfigMenu::printState() {
    if (mode == Closed || sensorCount == 0) {
        return;
    }

    IConfigurableSensor* sensor = sensors[selectedSensor];

    if (mode == SelectSensor) {
        output.print(F("[MENU] sensor: "));
        output.print(sensor->getSensorName());
        output.print(F(" ("));
        output.print((int)selectedSensor + 1);
        output.print(F("/"));
        output.print((int)sensorCount);
        output.println(F(")"));
        return;
    }

    SensorParameterMeta meta;
    if (!sensor->getParameterMeta(selectedParam, meta)) {
        return;
    }

    int value = sensor->getParameterValue(selectedParam);

    output.print(F("[MENU] "));
    output.print(sensor->getSensorName());
    output.print(F("."));
    output.print(meta.name);
    output.print(F(" = "));
    output.print(value);

    if (mode == EditParam) {
        output.println(F(" [edit]"));
    } else {
        output.println(F(" [select]"));
    }
}
