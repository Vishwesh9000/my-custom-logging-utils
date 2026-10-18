#pragma once
#include <optional>

enum class LogLevel: int {
    _DEFAULT = -1,
    NONE,
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    VERBOSE
};

struct LogMessage {
    int64_t timestamp; // In microseconds
    LogLevel level;
    char tag[16];
    char msg[120];
    std::optional<double> value;
    void assignTagAndMessage(const char* tag, const char* msg);
};