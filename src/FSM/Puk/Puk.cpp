/*
 * Puk.cpp
 *
 *  Created on: 08.01.2025
 *      Author: Marc
 */

#include "headers/Puk.h"

enum TimerSegment {
    INGRESS_VALID,
    INGRESS_EXPECTED,
    INGRESS_EXPIRED,
    HS_EXPECTED,
    HS_EXPIRED,
    SORTING_EXPECTED,
    SORTING_EXPIRED,
    SORTING_VALID,   
    EGRESS_EXPECTED,
    EGRESS_EXPIRED,
}


Puk::Puk(uint32_t pukId, TimerPlaces tp, ) {
    id = pukId;
}
    
Puk::~Puk() {}


uint32_t Puk::getPukId() {
    return id;
}

void Puk::startNewTimer(Timer timer){
    timer.start();

    timerlist.push_back(timer);
}
void Puk::startNewValidTimer(Timer timer){
    timer.start();
    valid = timer;
}


void Puk::updateTimersFast(){
    


}

// #define DURATION_INGRESS_DISTANCE_VALID_FAST std::chrono::milliseconds(1240) // guessed as DURATION_HS_PUK_EXPECTED_FAST / 2
// #define DURATION_INGRESS_DISTANCE_VALID_SLOW std::chrono::milliseconds(2790) // guessed as DURATION_HS_PUK_EXPECTED_SLOW / 2
void Puk::updateTimersSlow(TimerSegment seg){
    for (auto& timer : timerlist) {
        Logger::getInstance().log(LogLevel::DEBUG, "updateTimer", "Puk.updateTimersSlow");
        uint32_t progress = timer.getProgress();
        switch (seg){
        case INGRESS_VALID:
            duration = DURATION_INGRESS_VALID_SLOW;
            break;
        case INGRESS_EXPECTED:
            duration = DURATION_INGRESS_EXPECTED_SLOW;
            break;
        case INGRESS_EXPIRED:
            duration = DURATION_INGRESS_EXPIRED_SLOW;
            break;
        case HS_EXPECTED:
            duration = DURATION_HS_EXPECTED_SLOW;
            break;
        case HS_EXPIRED:
            duration = DURATION_HS_EXPIRED_SLOW;
            break;
        case SORTING_EXPECTED:
            duration = DURATION_SORTING_EXPECTED_SLOW;
            break;
        case SORTING_EXPIRED:
            duration = DURATION_SORTING_EXPIRED_SLOW;
            break;
        case SORTING_VALID:
            duration = DURATION_SORTING_VALID_SLOW;
            break;   
        case EGRESS_EXPECTED:
            duration = DURATION_EGRESS_EXPECTED_SLOW;
            break;
        case EGRESS_EXPIRED:
            duration = DURATION_EGRESS_EXPIRED_SLOW;
            break;
        default:
            break;
        }
    }
}

void Puk::updateTimersStop(TimerSegment seg){
    // calc Progress

} // ?

void Puk::deleteListTimers(){ 
    for (auto& timer : timerlist) {
        Logger::getInstance().log(LogLevel::DEBUG, "deleteTimer", "Puk.deleteListTimers"); 
        timer.remove();
    }
    timerlist.clear(); 
    } // not valid timer


