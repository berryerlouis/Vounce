#include "ConfigMenu.h"

ConfigMenu::ConfigMenu(
    ButtonInput& button,
    RotaryInput& rotary,
    Logger& logger,
    SensorConfigEeprom* configStore,
    MidiOut* midiOut,
    byte midiChannel)
    : button(button),
      rotary(rotary),
      logger(logger),
      configStore(configStore),
      midiOut(midiOut),
      midiChannel(midiChannel),
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

bool ConfigMenu::isEditing() const {
    return mode == EditParam;
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
    logger.info("Menu closed");
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
        if (next == current) {
            return;
        }

        sensor->setParameterValue(selectedParam, next);
        if (configStore != nullptr) {
            configStore->saveParameter(selectedSensor, selectedParam);
        }
        printState();
    }
}

void ConfigMenu::sendMenuMidi(byte value) {
    if (midiOut != nullptr) {
        midiOut->sendCC(midiChannel, 120, value);
    }
}

void ConfigMenu::printState() {
    if (mode == Closed || sensorCount == 0) {
        sendMenuMidi(0);
        return;
    }

    if (mode == SelectSensor) {
        sendMenuMidi(selectedSensor + 1);
        for (uint8_t i = 0; i < sensorCount; ++i) {
            IConfigurableSensor* sensor = sensors[i];
            String msg = String("Sensor: ") + sensor->getSensorName() + 
                        " (" + String((int)i + 1) + "/" + String((int)sensorCount) + ")";
            if(i == selectedSensor) {
                msg += " [selected]";
            }
            logger.info(msg);
        }
        return;
    }

    IConfigurableSensor* sensor = sensors[selectedSensor];
    if (mode == SelectParam) {
        sendMenuMidi(selectedParam + 100);
        for (uint8_t i = 0; i < sensor->getParameterCount(); ++i) {
            SensorParameterMeta meta;
            if (!sensor->getParameterMeta(i, meta)) {
                continue;
            }
            int value = sensor->getParameterValue(i);
            String msg = String(sensor->getSensorName()) + "." + String(meta.name) + 
                        " = " + String(value);
            if(i == selectedParam) {
                msg += " [selected]";
            }
            logger.info(msg);
        }
    } else if (mode == EditParam){
        SensorParameterMeta meta;
        if (!sensor->getParameterMeta(selectedParam, meta)) {
            return;
        }

        int value = sensor->getParameterValue(selectedParam);
        sendMenuMidi(constrain(value, 0, 127));

        String modeStr = "[edit]";
        String msg = String(sensor->getSensorName()) + "." + String(meta.name) + 
                    " = " + String(value) + " " + modeStr;
        logger.info(msg);
    }
}
