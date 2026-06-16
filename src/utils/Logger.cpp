#include "Logger.h"

Logger::Logger(Stream& output, Level minLevel, bool enabled)
    : output(output), minLevel(minLevel), enabled(enabled) {}

void Logger::setMinLevel(Level level) {
    minLevel = level;
}

void Logger::debug(const char* message) {
    log(DEBUG, message);
}

void Logger::debug(const String& message) {
    log(DEBUG, message.c_str());
}

void Logger::info(const char* message) {
    log(INFO, message);
}

void Logger::info(const String& message) {
    log(INFO, message.c_str());
}

void Logger::warning(const char* message) {
    log(WARNING, message);
}

void Logger::warning(const String& message) {
    log(WARNING, message.c_str());
}

void Logger::error(const char* message) {
    log(ERROR, message);
}

void Logger::error(const String& message) {
    log(ERROR, message.c_str());
}

void Logger::log(Level level, const char* message) {
    if (level < minLevel) {
        return;
    }
    printWithPrefix(level, message);
}

void Logger::log(Level level, const String& message) {
    log(level, message.c_str());
}

const char* Logger::getLevelPrefix(Level level) const {
    switch (level) {
        case DEBUG:   return "[DEBUG]  ";
        case INFO:    return "[INFO]   ";
        case WARNING: return "[WARN]   ";
        case ERROR:   return "[ERROR]  ";
        default:      return "[LOG]    ";
    }
}

void Logger::printWithPrefix(Level level, const char* message) {
    if (!enabled) {
        return;
    }
    output.print(getLevelPrefix(level));
    output.println(message);
}
