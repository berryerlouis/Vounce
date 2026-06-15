#pragma once
#include <Arduino.h>
#include "../core/IConfigurableSensor.h"
#include "../input/ButtonInput.h"
#include "../input/RotaryInput.h"

class ConfigMenu {
  public:
    static const uint8_t MaxSensors = 8;

    ConfigMenu(ButtonInput& button, RotaryInput& rotary, Stream& output);

    void begin();
    bool addSensor(IConfigurableSensor& sensor);
    void update();

    bool isOpen() const;

  private:
    enum Mode : uint8_t {
        Closed = 0,
        SelectSensor,
        SelectParam,
        EditParam
    };

    ButtonInput& button;
    RotaryInput& rotary;
    Stream& output;

    IConfigurableSensor* sensors[MaxSensors];
    uint8_t sensorCount;

    Mode mode;
    uint8_t selectedSensor;
    uint8_t selectedParam;

    void open();
    void close();

    void handleShortPress();
    void handleLongPress();
    void handleEncoderDelta(int delta);

    void printState();
};
