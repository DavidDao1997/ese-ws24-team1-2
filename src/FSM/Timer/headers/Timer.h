/*
 * Puk.h
 *
 *  Created on: 12.01.2025
 *      Author: Phillip
 */

#ifndef FSM_TIMER_HEADERS_TIMER_H_
#define FSM_TIMER_HEADERS_TIMER_H_

#include <chrono>
#include <sys/neutrino.h>
#include "../../../Logging/headers/Logger.h"

#define MILLION 1000000

class Timer {
public:

    enum PulseCode {
        // LBF_1_OPEN -> HS1_SAMPLE
        PULSE_INGRESS_1_DISTANCE_VALID,
        PULSE_HS_1_PUK_EXPECTED,
        PULSE_HS_1_PUK_EXPIRED,
        // HS1_SAMPLING_DONE -> LBM_1_INTERRUPTED
        PULSE_SORTING_1_PUK_EXPECTED,
        PULSE_SORTING_1_PUK_EXPIRED,
        PULSE_SORTING_1_TIMEOUT_DIVERTER,
        // LBM_1_OPEN -> LBE_1_INTERRUPTED
        PULSE_SORTING_1_DISTANCE_VALID,
        PULSE_EGRESS_1_PUK_EXPECTED,
        PULSE_EGRESS_1_PUK_EXPIRED,
        // LBE_1_OPEN -> LBF_2_INTERRUPTED
        PULSE_INGRESS_2_PUK_EXPECTED,
        PULSE_INGRESS_2_PUK_EXPIRED,
        // LBF_2_OPEN -> HS2_SAMPLE
        PULSE_HS_2_PUK_EXPECTED,
        PULSE_HS_2_PUK_EXPIRED,
        // HS2_SAMPLING_DONE -> LBM_2_INTERRUPTED
        PULSE_SORTING_2_PUK_EXPECTED,
        PULSE_SORTING_2_PUK_EXPIRED,
        PULSE_SORTING_2_TIMEOUT_DIVERTER,
        // LBM_2_OPEN -> LBE_2_INTERRUPTED
        PULSE_SORTING_2_DISTANCE_VALID,
        PULSE_EGRESS_2_PUK_EXPECTED,
        PULSE_EGRESS_2_PUK_EXPIRED
    };

    enum MotorState {
        MOTOR_FAST,
        MOTOR_SLOW,
        MOTOR_STOP
    };

    Timer();
    Timer(
        std::chrono::milliseconds fastDuration,
        std::chrono::milliseconds slowDuration,
        uint32_t connectionId,
        PulseCode pulseCode, 
        uint32_t pulseValue
    );
    Timer(
        std::chrono::milliseconds duration,
        uint32_t _connectionId,
        PulseCode _pulseCode,
        uint32_t _pulseValue
    );
    ~Timer();

    void setMotorState(MotorState motorState);
    void kill();
private:
    // pulse properties
    uint32_t connectionId;
    PulseCode pulseCode;
    uint32_t pulseValue;

    // timer properties
    timer_t timerId;
    // struct itimerspec timerSpec;
    MotorState motorState;
    std::chrono::milliseconds fastDuration;
    std::chrono::milliseconds slowDuration;
    // std::chrono::milliseconds currentDuration;
    uint8_t fractionRemaining; // isnt kept up to date but instead updatend when the timer is paused;
};





#endif /* FSM_TIMER_HEADERS_TIMER_H_ */














