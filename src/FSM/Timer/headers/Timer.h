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
        // std::queue<Puk*> heightSensor1;  // LBF_1_OPEN -> HS1_SAMPLE
        PULSE_INGRESS_1_DISTANCE_VALID = _PULSE_CODE_MINAVAIL + 1,
        PULSE_HS_1_PUK_EXPECTED,
        PULSE_HS_1_PUK_EXPIRED,
        // std::queue<Puk*> sorting1;       // HS1_SAMPLING_DONE -> LBM_1_INTERRUPTED
        PULSE_SORTING_1_PUK_EXPECTED,
        PULSE_SORTING_1_PUK_EXPIRED,
        // std::queue<Puk*> egress1;        // LBM_1_OPEN -> LBE_1_INTERRUPTED
        PULSE_SORTING_1_DISTANCE_VALID,
        PULSE_EGRESS_1_PUK_EXPECTED,
        PULSE_EGRESS_1_PUK_EXPIRED,
        // std::queue<Puk*> ingress2;      // LBE_1_OPEN -> LBF_2_INTERRUPTED
        PULSE_INGRESS_2_PUK_EXPECTED,
        PULSE_INGRESS_2_PUK_EXPIRED,
        // std::queue<Puk*> heightSensor2;  // LBF_2_OPEN -> HS2_SAMPLE
        PULSE_HS_2_PUK_EXPECTED,
        PULSE_HS_2_PUK_EXPIRED,
        // std::queue<Puk*> sorting2;       // HS2_SAMPLING_DONE -> LBM_2_INTERRUPTED
        PULSE_SORTING_2_PUK_EXPECTED,
        PULSE_SORTING_2_PUK_EXPIRED,
        // std::queue<Puk*> egress2;        // LBM_2_OPEN -> LBE_2_INTERRUPTED
        PULSE_EGRESS_2_PUK_EXPECTED,
        PULSE_EGRESS_2_PUK_EXPIRED
    };

    Timer();
    Timer(
        uint32_t connectionId,
        PulseCode pulseCode, 
        uint32_t pulseValue,
        std::chrono::milliseconds duration
    );
    ~Timer();

    // timer_t getTimerId();
    // struct itimerspec getTimerSpec();
    uint32_t getProgress();
    void setNewDuration(std::chrono::milliseconds duration);

    void start();
    void stop();
    void resume();
    void remove();
private:
    timer_t timerId;
    struct itimerspec timerSpec;

    std::chrono::milliseconds fullDuration;
};





#endif /* FSM_TIMER_HEADERS_TIMER_H_ */














