/*
 * PositionTracker.h
 *
 *  Created on: 11.01.2025
 *      Author: Marc
 */

#ifndef FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_
#define FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_

#include <queue>
#include <chrono>
#include <list>

#include "../../gen/src-gen/FSM.h"
#include "../../gen/src/sc_rxcpp.h"
#include "../../../Logging/headers/Logger.h"
#include "../../../Util/headers/Util.h"
#include "../../Puk/headers/Puk.h"
#include "../../Timer/headers/Timer.h"

// these are measured times by hand with a stopwatch, without any added margin
// #define DURATION_INGRESS_DISTANCE_VALID_FAST std::chrono::milliseconds(1240) // guessed as DURATION_HS_PUK_EXPECTED_FAST / 2
// #define DURATION_INGRESS_DISTANCE_VALID_SLOW std::chrono::milliseconds(2790) // guessed as DURATION_HS_PUK_EXPECTED_SLOW / 2
// #define DURATION_HS_PUK_EXPECTED_FAST std::chrono::milliseconds(2480)
// #define DURATION_HS_PUK_EXPECTED_SLOW std::chrono::milliseconds(5580)
// #define DURATION_HS_PUK_EXPIRED_FAST std::chrono::milliseconds(3500)
// #define DURATION_HS_PUK_EXPIRED_SLOW std::chrono::milliseconds(7400) // worst case measured, no margin added yet

#define DURATION_INGRESS_DISTANCE_VALID_FAST std::chrono::milliseconds(1000) // guessed as DURATION_HS_PUK_EXPECTED_FAST / 2
#define DURATION_INGRESS_DISTANCE_VALID_SLOW std::chrono::milliseconds(2000) // guessed as DURATION_HS_PUK_EXPECTED_SLOW / 2
#define DURATION_HS_PUK_EXPECTED_FAST std::chrono::milliseconds(3000)
#define DURATION_HS_PUK_EXPECTED_SLOW std::chrono::milliseconds(6000)
#define DURATION_HS_PUK_EXPIRED_FAST std::chrono::milliseconds(40000)
#define DURATION_HS_PUK_EXPIRED_SLOW std::chrono::milliseconds(80000) // worst case measured, no margin added yet
#define DURATION_SORTING_PUK_EXPECTED_FAST std::chrono::milliseconds(1001) // passed 400 ß: 40%
#define DURATION_SORTING_PUK_EXPECTED_SLOW std::chrono::milliseconds(2001)
#define DURATION_SORTING_PUK_EXPIRED_FAST std::chrono::milliseconds(3001)   
#define DURATION_SORTING_PUK_EXPIRED_SLOW std::chrono::milliseconds(4001) // worst case measured, no margin added yet 

// This could be solved by a boolean if we are sure there are no more profiles to track
enum HeightProfile {
    HEIGHTPROFLIE_INVALID,
    HEIGHTPROFLIE_VALID
};

struct TimerProps {
    timer_t timerid;
    struct itimerspec timer;
    // uint32_t progress;
};

class PositionTracker{
public:
    PositionTracker(FSM* fsm);
    ~PositionTracker();
    
    

private:
    enum MotorState {
        MOTOR_FAST,
        MOTOR_SLOW,
        MOTOR_STOP
    };

    FSM* fsm;
    void listen();
    std::thread* listenThread;
    uint32_t connectionId;

    std::queue<Puk*> heightSensor1;  // LBF_1_OPEN -> HS1_SAMPLE
    std::queue<Puk*> sorting1;       // HS1_SAMPLING_DONE -> LBM_1_INTERRUPTED
    std::queue<Puk*> egress1;        // LBM_1_OPEN -> LBE_1_INTERRUPTED

    std::queue<Puk*> ingress1;      // LBE_1_OPEN -> LBF_2_INTERRUPTED
    std::queue<Puk*> heightSensor2;  // LBF_2_OPEN -> HS2_SAMPLE
    std::queue<Puk*> sorting2;       // HS2_SAMPLING_DONE -> LBM_2_INTERRUPTED
    std::queue<Puk*> egress2;        // LBM_2_OPEN -> LBE_2_INTERRUPTED

    std::atomic<uint32_t> lastPukId;
    uint32_t nextPukId();

    
    std::atomic<MotorState> motorState1;
    std::atomic<MotorState> motorState2;

    void startTimer(Timer::PulseCode code, bool pukId, uint8_t festoId);
    std::chrono::milliseconds calcDuration(Timer::PulseCode code);
    std::queue<Puk*> updatePukQueue(std::queue<Puk*> tempQueue, MotorState ms);
};

class PositionTrackerObserver : public sc::rx::Observer<void> {
public:
    // Constructor that accepts a callback of type void() (a function with no parameters and no return value)
    PositionTrackerObserver(std::function<void()> callback): callback(callback) {}

    // Override the next() method to invoke the callback
    virtual void next() override {
        callback();
    }

private:
    std::function<void()> callback;
};


#endif /* FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_ */
