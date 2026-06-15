#pragma once
#include <Arduino.h>

struct SensorParameterMeta {
    const char* name;
    int minValue;
    int maxValue;
    int step;
};

class IConfigurableSensor {
  public:
    virtual ~IConfigurableSensor() = default;

    virtual const char* getSensorName() const = 0;
    virtual uint8_t getParameterCount() const = 0;
    virtual bool getParameterMeta(uint8_t index, SensorParameterMeta& meta) const = 0;
    virtual int getParameterValue(uint8_t index) const = 0;
    virtual void setParameterValue(uint8_t index, int value) = 0;
};
