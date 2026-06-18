#pragma once
#include <Arduino.h>
#include "../core/IConfigurableSensor.h"
#include "../core/SensorConfigEeprom.h"
#include "../utils/Logger.h"
#include "../led/Led.h"

class ConfigMenu {
  public:
    static const uint8_t MaxSensors = 8;
    static const uint8_t SerialCommandMaxLen = 31;

    ConfigMenu(
      Logger& logger,
      Stream& commandStream,
      Led& ledStatus,
      SensorConfigEeprom* configStore = nullptr
    );

    void begin();
    bool addSensor(IConfigurableSensor& sensor);
    void update();


  private:
    enum Mode : uint8_t {
        SelectSensor = 0,
        SelectParam,
        EditParam
    };

    Logger& logger;
    Stream& commandStream;
    Led& ledStatus;
    SensorConfigEeprom* configStore;

    IConfigurableSensor* sensors[MaxSensors];
    uint8_t sensorCount;

    Mode mode;
    uint8_t selectedSensor;
    uint8_t selectedParam;
    char serialCommandBuffer[SerialCommandMaxLen + 1];
    uint8_t serialCommandLength;

    void processSerialInput();
    void executeSerialCommand(const String& command);
    void printSerialHelp();

    void printState();
    void sendMenuMidi(byte value);
};
