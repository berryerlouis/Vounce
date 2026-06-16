#include "SensorConfigEeprom.h"
#include <EEPROM.h>

SensorConfigEeprom::SensorConfigEeprom(Logger* logger)
    : sensors{nullptr}, sensorCount(0), logger(logger) {}

bool SensorConfigEeprom::addSensor(IConfigurableSensor& sensor) {
    if (sensorCount >= MaxSensors) {
        return false;
    }

    sensors[sensorCount] = &sensor;
    sensorCount++;
    return true;
}

SensorConfigEeprom::BeginResult SensorConfigEeprom::begin() {
    logInfo(String("EEPROM: start (sensors=") + String(sensorCount) + ")");

    EepromHeader header;
    if (!readHeader(header)) {
        logWarning("EEPROM: cannot read header");
        return Error;
    }

    uint16_t parameterCount = getParameterCount();
    bool compatible =
        header.magic == Magic &&
        header.version == Version &&
        header.sensorCount == sensorCount &&
        header.parameterCount == parameterCount;

    if (!compatible) {
        logInfo("EEPROM: layout mismatch, writing defaults");
        if (!writeHeader()) {
            logWarning("EEPROM: header write failed");
            return Error;
        }
        if (!saveAll()) {
            logWarning("EEPROM: default save failed");
            return Error;
        }
        logInfo("EEPROM: defaults initialized");
        return InitializedDefaults;
    }

    if (!load()) {
        logWarning("EEPROM: load failed");
        return Error;
    }

    logInfo("EEPROM: values loaded");
    return LoadedFromEeprom;
}

bool SensorConfigEeprom::load() {
    if (getTotalBytesRequired() > EEPROM.length()) {
        logWarning("EEPROM: not enough space for load");
        return false;
    }

    for (uint8_t sensorIndex = 0; sensorIndex < sensorCount; ++sensorIndex) {
        IConfigurableSensor* sensor = sensors[sensorIndex];
        uint8_t paramCount = sensor->getParameterCount();

        for (uint8_t paramIndex = 0; paramIndex < paramCount; ++paramIndex) {
            int address = getParameterAddress(sensorIndex, paramIndex);
            if (address < 0 || address + (int)sizeof(int16_t) > EEPROM.length()) {
                logWarning("EEPROM: invalid parameter address during load");
                return false;
            }

            int16_t value = 0;
            EEPROM.get(address, value);
            sensor->setParameterValue(paramIndex, (int)value);
        }
    }

    return true;
}

bool SensorConfigEeprom::saveAll() {
    if (getTotalBytesRequired() > EEPROM.length()) {
        logWarning("EEPROM: not enough space for save");
        return false;
    }

    for (uint8_t sensorIndex = 0; sensorIndex < sensorCount; ++sensorIndex) {
        IConfigurableSensor* sensor = sensors[sensorIndex];
        uint8_t paramCount = sensor->getParameterCount();

        for (uint8_t paramIndex = 0; paramIndex < paramCount; ++paramIndex) {
            int address = getParameterAddress(sensorIndex, paramIndex);
            if (address < 0 || address + (int)sizeof(int16_t) > EEPROM.length()) {
                logWarning("EEPROM: invalid parameter address during save");
                return false;
            }

            int16_t value = (int16_t)sensor->getParameterValue(paramIndex);
            EEPROM.put(address, value);
        }
    }

    return true;
}

bool SensorConfigEeprom::saveParameter(uint8_t sensorIndex, uint8_t parameterIndex) {
    if (sensorIndex >= sensorCount) {
        logWarning("EEPROM: saveParameter sensor index out of range");
        return false;
    }

    IConfigurableSensor* sensor = sensors[sensorIndex];
    if (parameterIndex >= sensor->getParameterCount()) {
        logWarning("EEPROM: saveParameter parameter index out of range");
        return false;
    }

    int address = getParameterAddress(sensorIndex, parameterIndex);
    if (address < 0 || address + (int)sizeof(int16_t) > EEPROM.length()) {
        logWarning("EEPROM: saveParameter invalid address");
        return false;
    }

    int16_t value = (int16_t)sensor->getParameterValue(parameterIndex);
    EEPROM.put(address, value);
    return true;
}

uint16_t SensorConfigEeprom::getParameterCount() const {
    uint16_t total = 0;
    for (uint8_t i = 0; i < sensorCount; ++i) {
        total += sensors[i]->getParameterCount();
    }
    return total;
}

int SensorConfigEeprom::getParameterAddress(uint8_t sensorIndex, uint8_t parameterIndex) const {
    if (sensorIndex >= sensorCount) {
        return -1;
    }

    IConfigurableSensor* sensor = sensors[sensorIndex];
    if (parameterIndex >= sensor->getParameterCount()) {
        return -1;
    }

    uint16_t offset = 0;
    for (uint8_t i = 0; i < sensorIndex; ++i) {
        offset += sensors[i]->getParameterCount();
    }
    offset += parameterIndex;

    return getDataStartAddress() + (int)(offset * sizeof(int16_t));
}

int SensorConfigEeprom::getDataStartAddress() const {
    return (int)sizeof(EepromHeader);
}

int SensorConfigEeprom::getTotalBytesRequired() const {
    return getDataStartAddress() + (int)(getParameterCount() * sizeof(int16_t));
}

bool SensorConfigEeprom::readHeader(EepromHeader& header) const {
    if ((int)sizeof(EepromHeader) > EEPROM.length()) {
        return false;
    }

    EEPROM.get(0, header);
    return true;
}

bool SensorConfigEeprom::writeHeader() {
    EepromHeader header;
    header.magic = Magic;
    header.version = Version;
    header.sensorCount = sensorCount;
    header.parameterCount = getParameterCount();

    if ((int)sizeof(EepromHeader) > EEPROM.length()) {
        return false;
    }

    EEPROM.put(0, header);
    return true;
}

void SensorConfigEeprom::logInfo(const String& message) const {
    if (logger != nullptr) {
        logger->info(message);
    }
}

void SensorConfigEeprom::logWarning(const String& message) const {
    if (logger != nullptr) {
        logger->warning(message);
    }
}