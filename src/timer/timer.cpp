//timer.cpp

#include "timer.h"
#include "Arduino.h"
#include "logger/logger.h"

#define TXT_MOD "\x1b[2m"
// TODO: Add macro based timers
//TODO: Support logger.h macros
// TODO: streamline us/ms switching

Timer::Timer(std::string_view name, LogLevel level): 
        objectLogLevel((level == LogLevel::_DEFAULT ? defaultTimerLogLevel : level)), timerLogLevel(objectLogLevel), 
        objectName(name), 
        startTimeMS(millis()), startTimeUS(micros()), checkPointMS(startTimeMS), checkPointUS(startTimeUS)
         {
    if (!initDone) init();
    objTimer = true;
}

Timer::~Timer() {
    if (objTimer) {
        uint32_t endTimeMS = millis(), endTimeUS = micros();
        std::string_view name;
        if (!objectName.empty()) name = objectName;
        else name = "Timer";
        bool printMicros = endTimeMS-startTimeMS < 100;
        Logger::instance().log(tag, timerLogLevel, TXT_MOD "%.*s: %lu%s" TXT_MOD, static_cast<int>(name.length()), name.data(), 
                                        static_cast<unsigned long>(printMicros ? endTimeUS - startTimeUS : endTimeMS-startTimeMS), 
                                        printMicros ? "us" : "ms");
    }
}

void Timer::init() {
    if (!initDone) {
        Logger::init();
        Logger::instance().addTag(tag, initialLogLevel);
        initDone = true;
    }
}
void Timer::start(std::string_view name, LogLevel level) {
    if (funcTimer) LOG_W(tag, "Timer started twice before calling end()");
    if (level != LogLevel::_DEFAULT) timerLogLevel = level;
    else timerLogLevel = objectLogLevel;

    funcName = name;
    objTimer = false;
    funcTimer = true;
    startTimeMS = millis();
    startTimeUS = micros();
    checkPointMS = startTimeMS;
    checkPointUS = startTimeUS;
}

uint32_t Timer::recordCheckpoint(std::string_view checkPointName, LogLevel level) {
    if (!(funcTimer || objTimer)) {
        LOG_E(tag, "Can't record checkpoint: No timer running");
        return 0;
    }
    uint32_t timeDiffMS = millis() - checkPointMS, timeDiffUS = micros() - checkPointUS;
    checkPointMS = millis();
    checkPointUS = micros();

    if (level == LogLevel::_DEFAULT) level = timerLogLevel;

    std::string_view name;
    if (!funcName.empty()) name = funcName;
    else if (!objectName.empty()) name = objectName;
    else name = "Timer";
    
    bool printMicros = timeDiffMS < 100;
    Logger::instance().log(tag, level,TXT_MOD "%.*s: (%.*s) %lu%s" TXT_MOD, static_cast<int>(name.length()), name.data(), static_cast<int>(checkPointName.length()), checkPointName.data(), 
                                                static_cast<unsigned long>(printMicros ? timeDiffUS : timeDiffMS),
                                                printMicros ? "us" : "ms");
    return timeDiffMS;
}

uint32_t Timer::end() {
    if (!funcTimer) {
        LOG_E(tag, TXT_MOD "Timer::end() not executed: Timer not started" TXT_MOD);
        return 0;
    }
    uint32_t timeDiffMS = millis()-startTimeMS, timeDiffUS = micros()-startTimeUS;
    std::string_view name;
    if (!funcName.empty()) name = funcName;
    else if (!objectName.empty()) name = objectName;
    else name = "Timer";

    bool printMicros = timeDiffMS < 100;
    Logger::instance().log(tag, timerLogLevel,TXT_MOD "%.*s: %lu%s" TXT_MOD, static_cast<int>(name.length()), name.data(), 
                                    static_cast<unsigned long>(printMicros ? timeDiffUS : timeDiffMS),
                                    printMicros ? "us" : "ms");
    funcTimer = false;
    return timeDiffMS;
}