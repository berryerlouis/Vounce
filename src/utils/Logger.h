#pragma once
#include <Arduino.h>

class Logger {
  public:
    enum Level : uint8_t {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };

    explicit Logger(Stream& output, Level minLevel = INFO, bool enabled = true);

    void setMinLevel(Level level);
    void debug(const char* message);
    void debug(const String& message);
    void info(const char* message);
    void info(const String& message);
    void warning(const char* message);
    void warning(const String& message);
    void error(const char* message);
    void error(const String& message);

    void log(Level level, const char* message);
    void log(Level level, const String& message);

  private:
    Stream& output;
    Level minLevel;
    bool enabled;

    const char* getLevelPrefix(Level level) const;
    void printWithPrefix(Level level, const char* message);
};
