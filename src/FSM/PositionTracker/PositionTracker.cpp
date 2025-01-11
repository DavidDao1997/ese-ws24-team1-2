/*
 * PositionTracker.cpp
 *
 *  Created on: 11.01.2025
 *      Author: Marc
 */

#include "headers/PositionTracker.h"

PositionTracker::PositionTracker(FSM* fsm) {
    fsm->getFST_1_POSITION_INGRESS_NEW_PUK().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Logger::getInstance().log(LogLevel::DEBUG, "PositionTrackerObserver.next was called", "PositionTrackerObserver");
            })
        )
    );
    // fsm->raiseFST_1_POSITION_INGRESS_DISTANCE_VALID();
    // fsm->raiseFST_2_POSITION_INGRESS_PUK_EXPECTED();
    // fsm->raiseFST_1_POSITION_INGRESS_PUK_EXPIRED();


    // fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_NEW_PUK();
    // fsm->raiseFST_1_POSITION_INGRESS_DISTANCE_VALID();
    // fsm->raiseFST_2_POSITION_INGRESS_PUK_EXPECTED();
    // fsm->raiseFST_1_POSITION_INGRESS_PUK_EXPIRED();

    // fsm->getFST_1_POSITION_SORTING_NEW_PUK();
    // fsm->getFST_1_POSITION_EGRESS_NEW_PUK();
    // fsm->getFST_2_POSITION_INGRESS_NEW_PUK();
    // fsm->getFST_2_POSITION_HEIGHTMEASUREMENT_NEW_PUK();
    // fsm->getFST_2_POSITION_SORTING_NEW_PUK();
    // fsm->getFST_2_POSITION_EGRESS_NEW_PUK();
    
    // // someLoop() {
    // while (true) {
    //     rcvid = MsgRecievePulse(&msg)
        
    //     timers[
    // }
    // }

    // timer_t timerid;
    // struct sigevent event;
    // struct itimerspec timer;
    // int coid;
    // …
    // // Connection ID für Pulse Message
    // coid = ConnectAttach (0, 0, chid, 0, 0); // Fehlerbehandlung fehlt
    // // Definiere Pulse Event
    // SIGEV_PULSE_INIT (&event, coid, SIGEV_PULSE_PRIO_INHERIT, DIS_VAL/PUK_EXPE/PUK_, 0);
    // // Erzeuge den Timer
    // timer_create (CLOCK_REALTIME, &event, &timerid) // Fehlerbehandlung fehlt
    // // Setup und Start eines periodischen Timers
    // timer.it_value.tv_sec = 1;
    // timer.it_value.tv_nsec = 0;
    // timer.it_interval.tv_sec = 1;
    // timer.it_interval.tv_nsec = 0;
    // timer_settime (timerid, 0, &timer, NULL);
}

PositionTracker::~PositionTracker() {}
