#pragma once
#include <Arduino.h>
#include "IConfigurableSensor.h"
#include "../utils/Logger.h"

class SensorConfigEeprom {
  public:
    static const uint8_t MaxSensors = 8;

    enum BeginResult : uint8_t {
        LoadedFromEeprom = 0,
        InitializedDefaults,
        Error
    };

    SensorConfigEeprom(Logger* logger = nullptr);

    bool addSensor(IConfigurableSensor& sensor);
    BeginResult begin();
    bool load();
    bool saveAll();
    bool saveParameter(uint8_t sensorIndex, uint8_t parameterIndex);

  private:
    static const uint16_t Magic = 0x5643;
    static const uint8_t Version = 1;

    struct EepromHeader {
        uint16_t magic;
        uint8_t version;
        uint8_t sensorCount;
        uint8_t parameterCount;
    };

    IConfigurableSensor* sensors[MaxSensors];
    uint8_t sensorCount;
    Logger* logger;

    uint16_t getParameterCount() const;
    int getParameterAddress(uint8_t sensorIndex, uint8_t parameterIndex) const;
    int getDataStartAddress() const;
    int getTotalBytesRequired() const;

    bool readHeader(EepromHeader& header) const;
    bool writeHeader();

    void logInfo(const String& message) const;
    void logWarning(const String& message) const;
};