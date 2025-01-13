#include "headers/Timer.h"


Timer::Timer() {}
Timer::Timer(
    uint32_t connectionId,
    PulseCode pulseCode, 
    uint32_t pulseValue,
    std::chrono::milliseconds duration
) {
    struct sigevent event;
    SIGEV_PULSE_INIT(&event, connectionId, SIGEV_PULSE_PRIO_INHERIT, pulseCode, pulseValue);
    timer_create (CLOCK_REALTIME, &event, &timerId); // Fehlerbehandlung fehlt
    
    // Definiere Pulse Event
   
    // Erzeuge den Timer
    // Setup und Start eines periodischen Timers
    timerSpec.it_value.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    timerSpec.it_value.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration % 1000).count();
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;

    fullDuration = duration;
    // timer_settime (timerProps.timerid, 0, &timerProps.timer, NULL);
    // timer_delete(timerProps.timerid);
    // timer_settime (timerProps.timerid, 0, &timerProps.timer, NULL);
}

Timer::~Timer() {}

uint32_t Timer::getProgress() {
    struct itimerspec timerStatus;
	if(timer_gettime(timerId,&timerStatus) != 0){
		perror("[Client] Getting timer failed");
	}
    std::chrono::milliseconds s = std::chrono::milliseconds(timerStatus.it_value.tv_sec *1000);
    std::chrono::milliseconds ms = std::chrono::milliseconds(timerStatus.it_value.tv_nsec / MILLION);
    std::chrono::milliseconds timeLeft = s + ms;


    if (fullDuration.count() == 0) {
        // Handle division by zero if fullDuration is zero
        Logger::getInstance().log(LogLevel::DEBUG, "IS ZERO FullTime:" + std::to_string(fullDuration.count()) + "ms", "Timer");
        return 0;
    }

    // Calculate the elapsed time
    auto elapsedTime = fullDuration - timeLeft;  // Time that has passed
    uint32_t progress = static_cast<uint8_t>((elapsedTime.count() * UINT8_MAX) / fullDuration.count());
    Logger::getInstance().log(LogLevel::DEBUG, "FullTime:" + std::to_string(fullDuration.count()) + "ms", "Timer");
    Logger::getInstance().log(LogLevel::DEBUG, "TimeLeft:" + std::to_string(timeLeft.count()) + "ms", "Timer");
    Logger::getInstance().log(LogLevel::DEBUG, "TimeElapsed:" + std::to_string(elapsedTime.count()) + "ms", "Timer");
    Logger::getInstance().log(LogLevel::DEBUG, "Progress:" + std::to_string(progress) + "/" + std::to_string(UINT8_MAX), "Timer");

    return progress;   
}


void Timer::setNewDuration(std::chrono::milliseconds duration) {}

void Timer::start() { timer_settime (timerId, 0, &timerSpec, NULL); }
void Timer::stop() {}
void Timer::resume() {}
void Timer::remove(){ timer_delete(timerId); }