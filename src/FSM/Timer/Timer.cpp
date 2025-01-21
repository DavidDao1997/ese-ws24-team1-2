#include "headers/Timer.h"


Timer::Timer() {}
Timer::Timer(
    std::chrono::milliseconds _fastDuration,
    std::chrono::milliseconds _slowDuration,
    uint32_t _connectionId,
    PulseCode _pulseCode,
    uint32_t _pulseValue
) {
    fastDuration = _fastDuration;
    slowDuration = _slowDuration;
    connectionId = _connectionId;
    pulseCode = _pulseCode;
    pulseValue = _pulseValue;
    motorState = MOTOR_STOP;
    fractionRemaining = UINT8_MAX;

    Logger::getInstance().log(LogLevel::TRACE, "New Timer", "Timer");
}

Timer::~Timer() {}

void Timer::setMotorState(MotorState nextMotorState) {
    if (fractionRemaining == 0) return;
    if (motorState == nextMotorState) {
        Logger::getInstance().log(LogLevel::TRACE, "Motor state unchanged, Progress: " + std::to_string(static_cast<uint8_t>(((UINT8_MAX - fractionRemaining) * 100) / 255)) + "%", "Timer");
        return;
    };

    // starting
    if (motorState == MOTOR_STOP) {
        struct sigevent event;
        // setup pulse
        SIGEV_PULSE_INIT(&event, connectionId, SIGEV_PULSE_PRIO_INHERIT, pulseCode, pulseValue);
        timer_create (CLOCK_REALTIME, &event, &timerId); // Fehlerbehandlung fehlt
        
        // setup timer
        std::chrono::milliseconds fullDuration = nextMotorState == MOTOR_FAST ? fastDuration : slowDuration;
        uint32_t timeRemaining = fullDuration.count() * fractionRemaining/UINT8_MAX; // in milliseconds
        struct itimerspec timerSpec = {};
        timerSpec.it_value.tv_sec = timeRemaining / 1000;
        timerSpec.it_value.tv_nsec = (timeRemaining % 1000) * MILLION;
        timerSpec.it_interval.tv_sec = 0;
        timerSpec.it_interval.tv_nsec = 0;

        timer_settime (timerId, 0, &timerSpec, NULL);
        // TODO catch error
        motorState = nextMotorState;
        Logger::getInstance().log(
            LogLevel::TRACE, 
            "Timer started\n\tSpeed: " +
            std::string(nextMotorState == MOTOR_FAST ? "fast" : "slow") + "\n\tProgress: " +
            std::to_string(static_cast<uint8_t>(((UINT8_MAX - fractionRemaining) * 100) / 255)) + "%\n\tTime: " +
            std::to_string(timeRemaining) + "ms\n\tPulse: " +
            std::to_string(pulseCode),  
            "Timer" 
        );
        return;
    }

    struct itimerspec timerSpec = {};
    if(timer_gettime(timerId,&timerSpec) != 0){
        Logger::getInstance().log(LogLevel::ERROR, "timer_gettime failed", "Timer");
    }
    uint32_t timeRemaining = timerSpec.it_value.tv_sec * 1000 + timerSpec.it_value.tv_nsec / MILLION; // in milliseconds
    uint32_t timeTotal = motorState == MOTOR_FAST ? fastDuration.count()  : slowDuration.count(); // in milliseconds
    if (timeTotal == 0) {
        Logger::getInstance().log(LogLevel::ERROR, "timeTotal = o, div by 0", "Timer");
        return;
    }
    fractionRemaining = static_cast<uint8_t>(timeRemaining * UINT8_MAX / timeTotal);
    if (fractionRemaining == 0) return;

    // stopping
    if (nextMotorState == MOTOR_STOP) {
        timer_delete(timerId);
        // TODO catch error
        timerId  = 0;
        motorState = nextMotorState;
        Logger::getInstance().log(
            LogLevel::TRACE, 
            "Timer stopped\n\tProgress: " + 
            std::to_string(static_cast<uint8_t>(((UINT8_MAX - fractionRemaining) * 100) / 255)) + "%\n\tTime: " +
            std::to_string(timeRemaining) + "ms\n\tPulse: " +
            std::to_string(pulseCode), 
            "Timer" 
        );
        return;
    }

    // changing
    uint32_t nextTimeTotal = nextMotorState == MOTOR_FAST ? fastDuration.count()  : slowDuration.count(); // in milliseconds
    uint32_t nextTimeRemaining = (uint32_t)fractionRemaining * nextTimeTotal / UINT8_MAX;
    struct itimerspec nextTimerSpec = {};
    nextTimerSpec.it_value.tv_sec = nextTimeRemaining / 1000;
    nextTimerSpec.it_value.tv_nsec = (nextTimeRemaining % 1000) * MILLION;
    timer_settime (timerId, 0, &nextTimerSpec, NULL);
    motorState = nextMotorState;
    Logger::getInstance().log(
        LogLevel::TRACE, 
        "Timer speed updated\n\tspeed: " +
        std::string(nextMotorState == MOTOR_FAST ? "fast" : "slow") + "\n\tProgress: " + 
        std::to_string(static_cast<uint8_t>(((UINT8_MAX - fractionRemaining) * 100) / 255)) + "%\n\ttimeRemaining: " +
        std::to_string(nextTimeRemaining) + "ms\n\tPulse: " +
        std::to_string(pulseCode), 
        "Timer" 
    );
}

void Timer::kill(){ timer_delete(timerId); }