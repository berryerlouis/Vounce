#pragma once
#include <Arduino.h>
#include "../core/IConfigurableSensor.h"
#include "../core/SensorConfigEeprom.h"
#include "../input/ButtonInput.h"
#include "../input/RotaryInput.h"
#include "../utils/Logger.h"
#include "../midi/MidiOut.h"

class ConfigMenu {
  public:
    static const uint8_t MaxSensors = 8;

    ConfigMenu(
      ButtonInput& button,
      RotaryInput& rotary,
      Logger& logger,
      SensorConfigEeprom* configStore = nullptr,
      MidiOut* midiOut = nullptr,
      byte midiChannel = 11
    );

    void begin();
    bool addSensor(IConfigurableSensor& sensor);
    void update();

    bool isOpen() const;
    bool isEditing() const;  // Returns true when in EditParam mode

  private:
    enum Mode : uint8_t {
        Closed = 0,
        SelectSensor,
        SelectParam,
        EditParam
    };

    ButtonInput& button;
    RotaryInput& rotary;
    Logger& logger;
    SensorConfigEeprom* configStore;
    MidiOut* midiOut;
    byte midiChannel;

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
    void sendMenuMidi(byte value);
};
