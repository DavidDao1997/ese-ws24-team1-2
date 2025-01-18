/*
 * PositionTracker.cpp
 *
 *  Created on: 11.01.2025
 *      Author: Marc
 */

#include "headers/PositionTracker.h"

PositionTracker::PositionTracker(FSM* _fsm) {
    fsm = _fsm;
    listenThread = new std::thread(std::bind(&PositionTracker::listen, this));
    motorState1.store(Timer::MotorState::MOTOR_STOP);
    motorState2.store(Timer::MotorState::MOTOR_STOP);
    isMetalDesired1 = false;
    isMetalDesired2 = false;
    onEvent(&fsm->getFST_1_POSITION_INGRESS_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lock(heightSensor1Mutex);

        Puk* puk = new Puk(nextPukId());              
        heightSensor1.push(puk);

        puk->approachingHS(
            motorState1.load(),
            new Timer(
                getDuration(SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_SLOW), 
                connectionId, 
                Timer::PulseCode::PULSE_INGRESS_1_DISTANCE_VALID, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW), 
                connectionId, 
                Timer::PulseCode::PULSE_HS_1_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW), 
                connectionId, 
                Timer::PulseCode::PULSE_HS_1_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onIngressNewPuk");
    });
    onEvent(&fsm->getFST_1_PUK_HEIGHT_IS_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        Puk* puk = sorting1.front();  // FST1 raises "puk valid/inValid" after "sorting new puk", so we need to use sorting1.front() here
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onIsValid");
            return;
        }
        puk->setIsValid(true);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] HEIGHT_IS_VALID", "PositionTracker.onIsValid");
    });
    onEvent(&fsm->getFST_1_PUK_HEIGHT_IS_NOT_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        Puk* puk = sorting1.front();
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onIsNotValid");
            return;
        }
        puk->setIsValid(false); // FIXME add HS mocking in tests to make this testable
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] HEIGHT_IS_NOT_VALID", "PositionTracker.onIsNotValid");
    });
    // TODO Puk that disappear or are sorted out
    onEvent(&fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = PositionTracker::queuePop(&heightSensor1);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onHSNewPuk");
            return;
        }
        sorting1.push(puk);

        puk->approachingSorting(
            motorState1.load(),
            new Timer(
                getDuration(SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_1_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_1_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onHSNewPuk");
    });
    onEvent(&fsm->getFST_1_PUK_IS_METAL(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = sorting1.front();  // FST1 raises "puk valid/inValid" after "sorting new puk", so we need to use sorting1.front() here
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onIsMetal");
            return;
        }
        if (isMetalDesired1 && puk->getIsValid()) {
            // passthrough
            puk->setIsMetal(true);
            isMetalDesired1 = false;
            fsm->raiseFST_1_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Passing", "PositionTracker.onIsMetal");
        } else {
            // eject
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_1_PUK_SORTING_EJECT();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Ejecting", "PositionTracker.onIsMetal");
        }
    });
    onEvent(&fsm->getFST_1_PUK_IS_NOT_METAL(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = sorting1.front();
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onIsNotMetal");
            return;
        }
        if (!isMetalDesired1 && puk->getIsValid()) {
            // passthrough
            puk->setIsMetal(false);
            isMetalDesired1 = true;
            fsm->raiseFST_1_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Passing", "PositionTracker.onIsNotMetal");
        } else {
            // eject
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_1_PUK_SORTING_EJECT();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Ejecting", "PositionTracker.onIsNotMetal");
        }
    });
    onEvent(&fsm->getFST_1_POSITION_SORTING_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);
        std::lock_guard<std::mutex> lockEgress(egress1Mutex);

        Puk* puk = PositionTracker::queuePop(&sorting1);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onSortingNewPuk");
            return;
        }
        egress1.push(puk);

        puk->approachingEgress(
            motorState1.load(),
            new Timer(
                getDuration(SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_FAST),
                getDuration(SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_1_DISTANCE_VALID, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_1_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_1_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onSortingNewPuk");
    });
    onEvent(&fsm->getFST_1_POSITION_EGRESS_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockEgress(egress1Mutex);
        std::lock_guard<std::mutex> lockSorting(ingress2Mutex);

        Puk* puk = queuePop(&egress1);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onEgressNewPuk");
            return;
        }
        ingress2.push(puk);

        puk->approachingIngress(
            motorState1.load(),
            new Timer(
                getDuration(SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_INGRESS_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_INGRESS_2_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onEgressNewPuk");
    });
    onEvent(&fsm->getFST_2_POSITION_INGRESS_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockSorting(ingress2Mutex);
        std::lock_guard<std::mutex> lockEgress(heightSensor2Mutex);

        Puk* puk = queuePop(&ingress2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onIngressNewPuk");
            return;
        }
        heightSensor2.push(puk);
        
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2] " 
            + std::to_string(heightSensor1.size())
            + std::to_string(sorting1.size())
            + std::to_string(egress1.size())
            + std::to_string(ingress2.size())
            + std::to_string(heightSensor2.size())
            + std::to_string(sorting2.size())
            + std::to_string(egress2.size()),
            "PositionTracker.onIngressNewPuk");
        puk->approachingHS(
            motorState2.load(),
            nullptr,
            new Timer(
                getDuration(SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_HS_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_HS_2_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIngressNewPuk");
    });
    onEvent(&fsm->getFST_2_PUK_HEIGHT_IS_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        Puk* puk = heightSensor2.front();
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onIsValid");
            return;
        }
        if (!puk->getIsValid()) {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Profile mismatch", "PositionTracker.onIsValid");
        } else {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIsValid");
        }
    });
    onEvent(&fsm->getFST_2_PUK_HEIGHT_IS_NOT_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        Puk* puk = heightSensor2.front();
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onIsInvalid");
            return;
        }
        if (puk->getIsValid()) {
            puk->setIsValid(false);
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Profile mismatch, invalidating puk", "PositionTracker.onIsInvalid");

        }
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIsInvalid");
    });
    onEvent(&fsm->getFST_2_POSITION_HEIGHTMEASUREMENT_NEW_PUK(), [this](){
        Logger::getInstance().log(LogLevel::DEBUG, "ENTRY HS [FST2]", "PositionTracker.onHSNewPuk");
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = queuePop(&heightSensor2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onHSNewPuk");
            return;
        }
        sorting2.push(puk);

        puk->approachingSorting(
            motorState2.load(),
            new Timer(
                getDuration(SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_2_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onHSNewPuk");
    });
    onEvent(&fsm->getFST_2_PUK_IS_METAL(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting2Mutex);

        Puk* puk = sorting2.front();
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onIsMetal");
            return;
        }
        if (isMetalDesired2 && puk->getIsMetal() && puk->getIsValid()) {
            // passthrough
            isMetalDesired2 = false;
            fsm->raiseFST_2_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Passing", "PositionTracker.onIsMetal");
        } else {
            // eject
            sorting2.pop();
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_2_PUK_SORTING_EJECT();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Ejecting", "PositionTracker.onIsMetal");
        }
    });
    onEvent(&fsm->getFST_2_PUK_IS_NOT_METAL(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting2Mutex);

        Puk* puk = sorting2.front();
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onIsNotMetal");
            return;
        }
        if (!isMetalDesired2 && !puk->getIsMetal() && puk->getIsValid()) {
            // passthrough
            puk->setIsMetal(false);
            isMetalDesired2 = true;
            fsm->raiseFST_2_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Passing", "PositionTracker.onIsNotMetal");
            return;
        }

        // eject
        if (puk->getIsMetal()) {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] EjectionReason: Metal mismatch", "PositionTracker.onIsNotMetal");
        } else {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Ejecting", "PositionTracker.onIsNotMetal");
        }
        sorting2.pop();
        puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
        fsm->raiseFST_2_PUK_SORTING_EJECT();
    });
    onEvent(&fsm->getFST_2_POSITION_SORTING_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting2Mutex);
        std::lock_guard<std::mutex> lockHeightSensor(egress2Mutex);

        Puk* puk = queuePop(&sorting2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onSortingNewPuk");
            return;
        }
        egress2.push(puk);

        puk->approachingEgress(
            motorState2.load(),
            new Timer(
                getDuration(SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_FAST),
                getDuration(SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_2_DISTANCE_VALID, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_2_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onSortingNewPuk");
    });
    onEvent(&fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(heightSensor1Mutex);
        Puk* puk = queuePop(&heightSensor1);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onHSPukRemoved");
            return;
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onHSPukRemoved");
    });
    onEvent(&fsm->getFST_1_POSITION_SORTING_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(sorting1Mutex);
        Puk* puk = queuePop(&sorting1);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onSortingPukRemoved");
            return;
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onSortingPukRemoved");
    });
    onEvent(&fsm->getFST_1_POSITION_EGRESS_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(egress1Mutex);
        Puk* puk = queuePop(&egress1);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onEgressPukRemoved");
            return;
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onEgressPukRemoved");
    });
    onEvent(&fsm->getFST_2_POSITION_INGRESS_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(ingress2Mutex);
        Puk* puk = queuePop(&ingress2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onIngressPukRemoved");
            return;
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIngressPukRemoved");
    });
    onEvent(&fsm->getFST_2_POSITION_HEIGHTMEASUREMENT_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(heightSensor2Mutex);
        Puk* puk = queuePop(&heightSensor2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onHSPukRemoved");
            return;
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onHSPukRemoved");
    });
    onEvent(&fsm->getFST_2_POSITION_SORTING_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(sorting2Mutex);
        Puk* puk = queuePop(&sorting2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onSortingPukRemoved");
            return;
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onSortingPukRemoved");
    });
    onEvent(&fsm->getFST_2_POSITION_EGRESS_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(egress2Mutex);
        Puk* puk = queuePop(&egress2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] nullpointer exception", "PositionTracker.onEgressPukRemoved");
            return;
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP);
        Logger::getInstance().log(LogLevel::TRACE, "[FST2]", "PositionTracker.onEgressPukRemoved");
    });
    onEvent(&fsm->getESTOP_RECEIVED(), [this](){
        // kill all remaining timers
        updatePukQueue(heightSensor1, Timer::MotorState::MOTOR_STOP);
        updatePukQueue(sorting1, Timer::MotorState::MOTOR_STOP);
        updatePukQueue(egress1, Timer::MotorState::MOTOR_STOP);
        updatePukQueue(ingress2, Timer::MotorState::MOTOR_STOP);
        updatePukQueue(heightSensor2, Timer::MotorState::MOTOR_STOP);
        updatePukQueue(sorting2, Timer::MotorState::MOTOR_STOP);
        updatePukQueue(egress2, Timer::MotorState::MOTOR_STOP);

        std::lock_guard<std::mutex> lockHeightSensor1(heightSensor1Mutex);
        std::lock_guard<std::mutex> lockSorting1(sorting1Mutex);
        std::lock_guard<std::mutex> lockEgress1(egress1Mutex);
        std::lock_guard<std::mutex> lockIngress2(ingress2Mutex);
        std::lock_guard<std::mutex> lockHeightSensor2(heightSensor2Mutex);
        std::lock_guard<std::mutex> lockSorting2(sorting2Mutex);
        std::lock_guard<std::mutex> lockEgress2(egress2Mutex);

        // clear all segements
        heightSensor1 = *new std::queue<Puk*>();
        sorting1 = *new std::queue<Puk*>();
        egress1 = *new std::queue<Puk*>();
        ingress2 = *new std::queue<Puk*>();
        heightSensor2 = *new std::queue<Puk*>();
        sorting2 = *new std::queue<Puk*>();
        egress2 = *new std::queue<Puk*>();
        isMetalDesired1 = false;
        isMetalDesired2 = false;
    });
    onEvent(&fsm->getMOTOR_1_FAST(), [this](){
        handleMotorChange(FESTO1, Timer::MotorState::MOTOR_FAST);
    });
    onEvent(&fsm->getMOTOR_1_SLOW(), [this](){
        handleMotorChange(FESTO1, Timer::MotorState::MOTOR_SLOW);
    });
    onEvent(&fsm->getMOTOR_1_STOP(), [this](){
        handleMotorChange(FESTO1, Timer::MotorState::MOTOR_STOP);
    });
    onEvent(&fsm->getMOTOR_2_FAST(), [this](){
        handleMotorChange(FESTO2, Timer::MotorState::MOTOR_FAST);
    });
    onEvent(&fsm->getMOTOR_2_SLOW(), [this](){
        handleMotorChange(FESTO2, Timer::MotorState::MOTOR_SLOW);
    });
    onEvent(&fsm->getMOTOR_2_STOP(), [this](){
        handleMotorChange(FESTO2, Timer::MotorState::MOTOR_STOP);
    });
}

void PositionTracker::handleMotorChange(uint8_t festoId, Timer::MotorState motorState) {
    if (festoId == FESTO1) {
        motorState1.store(motorState);
        {
            std::lock_guard<std::mutex> lock(egress1Mutex);
            egress1 = updatePukQueue(egress1, motorState);
        }
        {
            std::lock_guard<std::mutex> lock(sorting1Mutex);
            sorting1 = updatePukQueue(sorting1, motorState);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor1Mutex);
            heightSensor1 = updatePukQueue(heightSensor1, motorState);
        }
    } else {
        motorState2.store(motorState);
        {
            std::lock_guard<std::mutex> lock(egress2Mutex);
            egress2 = updatePukQueue(egress2, motorState);
        }
        {
            std::lock_guard<std::mutex> lock(sorting2Mutex);
            sorting2 = updatePukQueue(sorting2, motorState);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor2Mutex);
            heightSensor2 = updatePukQueue(heightSensor2, motorState);
        }
        {
            std::lock_guard<std::mutex> lock(ingress2Mutex);
            ingress2 = updatePukQueue(ingress2, motorState);
        }
    }
    std::string motorStateDescriptor;
    if (motorState == Timer::MotorState::MOTOR_FAST) motorStateDescriptor = "Fast";
    if (motorState == Timer::MotorState::MOTOR_SLOW) motorStateDescriptor = "Slow";
    if (motorState == Timer::MotorState::MOTOR_STOP) motorStateDescriptor = "Stop";
    Logger::getInstance().log(LogLevel::TRACE, "[FST" + std::to_string(festoId + 1) + "] Motor " + motorStateDescriptor, "PositionTracker.onMotor" + motorStateDescriptor);
}

PositionTracker::~PositionTracker() {
}

std::queue<Puk*> PositionTracker::updatePukQueue(std::queue<Puk*> tempQueue, Timer::MotorState ms){
    std::queue<Puk*> updatedQueue;// = new std::queue<Puk*>();
    while (!tempQueue.empty()) {
        Puk* puk = tempQueue.front();
        Logger::getInstance().log(LogLevel::TRACE,  "Processing Puk with ID: " + std::to_string(puk->getPukId()), "PositionTracker.updatePukQueue");
        puk->setTimers(ms);
        tempQueue.pop();
        updatedQueue.push(puk);
    }

    return updatedQueue;
}

Puk* PositionTracker::queuePop(std::queue<Puk*>* queue){
    if (queue->empty()) {
        Logger::getInstance().log(LogLevel::DEBUG,  "Queue ist EMPTY!", "PositionTracker.queuePop");
        return nullptr;
    }
    Puk* puk = queue->front();
    queue->pop();
    return puk;
}

// void PositionTracker::reset() {
//     isMetalDesired1 = false;
//     isMetalDesired2 = false;
//     updatePukQueue(heightSensor1, Timer::MotorStaTe::MOTOR_STOP)
// }

void PositionTracker::listen() {
    // Connection ID für Pulse Message
    uint32_t channelId = ChannelCreate(0);
    connectionId = ConnectAttach(0, 0, channelId, 0, 0); // Fehlerbehandlung fehlt
    _pulse msg;

    while (1) {
        MsgReceive(channelId, &msg, sizeof(msg), NULL);
        std::string pulseName = "";
        switch (msg.code) {
            case Timer::PulseCode::PULSE_INGRESS_1_DISTANCE_VALID:
                pulseName = "PULSE_INGRESS_1_DISTANCE_VALID";
                fsm->raiseFST_1_POSITION_INGRESS_DISTANCE_VALID();
                break;
            case Timer::PulseCode::PULSE_HS_1_PUK_EXPECTED:
                pulseName = "PULSE_HS_1_PUK_EXPECTED";
                fsm->raiseFST_1_POSITION_HEIGHTMEASUREMENT_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_HS_1_PUK_EXPIRED:
                pulseName = "PULSE_HS_1_PUK_EXPIRED";
                fsm->raiseFST_1_POSITION_HEIGHTMEASUREMENT_PUK_EXPIRED();
                break;
            case Timer::PulseCode::PULSE_SORTING_1_PUK_EXPECTED:
                pulseName = "PULSE_SORTING_1_PUK_EXPECTED";
                fsm->raiseFST_1_POSITION_SORTING_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_SORTING_1_PUK_EXPIRED:
                pulseName = "PULSE_SORTING_1_PUK_EXPIRED";
                fsm->raiseFST_1_POSITION_SORTING_PUK_EXPIRED();
                break;
            case Timer::PulseCode::PULSE_SORTING_1_DISTANCE_VALID:
                pulseName = "PULSE_SORTING_1_DISTANCE_VALID";
                fsm->raiseFST_1_POSITION_DIVERTER_DISTANCE_VALID();
                break;
            case Timer::PulseCode::PULSE_EGRESS_1_PUK_EXPECTED:
                pulseName = "PULSE_EGRESS_1_PUK_EXPECTED";
                fsm->raiseFST_1_POSITION_EGRESS_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_EGRESS_1_PUK_EXPIRED:
                pulseName = "PULSE_EGRESS_1_PUK_EXPIRED";
                fsm->raiseFST_1_POSITION_EGRESS_PUK_EXPIRED();
                break;
            case Timer::PulseCode::PULSE_INGRESS_2_PUK_EXPECTED:
                pulseName = "PULSE_INGRESS_2_PUK_EXPECTED";
                fsm->raiseFST_2_POSITION_INGRESS_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_INGRESS_2_PUK_EXPIRED:
                pulseName = "PULSE_INGRESS_2_PUK_EXPIRED";
                fsm->raiseFST_2_POSITION_INGRESS_PUK_EXPIRED();
                break;
            case Timer::PulseCode::PULSE_HS_2_PUK_EXPECTED:
                pulseName = "PULSE_HS_2_PUK_EXPECTED";
                fsm->raiseFST_2_POSITION_HEIGHTMEASUREMENT_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_HS_2_PUK_EXPIRED:
                pulseName = "PULSE_HS_2_PUK_EXPIRED";
                fsm->raiseFST_2_POSITION_HEIGHTMEASUREMENT_PUK_EXPIRED();
                break;
            case Timer::PulseCode::PULSE_SORTING_2_PUK_EXPECTED:
                pulseName = "PULSE_SORTING_2_PUK_EXPECTED";
                fsm->raiseFST_2_POSITION_SORTING_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_SORTING_2_PUK_EXPIRED:
                pulseName = "PULSE_SORTING_2_PUK_EXPIRED";
                fsm->raiseFST_2_POSITION_SORTING_PUK_EXPIRED();
                break;
            case Timer::PulseCode::PULSE_SORTING_2_DISTANCE_VALID:
                pulseName = "PULSE_SORTING_2_DISTANCE_VALID";
                fsm->raiseFST_2_POSITION_DIVERTER_DISTANCE_VALID();
                break;
            case Timer::PulseCode::PULSE_EGRESS_2_PUK_EXPECTED:
                pulseName = "PULSE_EGRESS_2_PUK_EXPECTED";
                fsm->raiseFST_2_POSITION_EGRESS_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_EGRESS_2_PUK_EXPIRED:
                pulseName = "PULSE_EGRESS_2_PUK_EXPIRED";
                fsm->raiseFST_2_POSITION_EGRESS_PUK_EXPIRED();
                break;
            default:
                pulseName = std::to_string(msg.code);
                break;
        }
        Logger::getInstance().log(LogLevel::DEBUG, "Message recieved\n\tcode: " + pulseName + "\n\tvalue: " + std::to_string(msg.value.sival_int), "PositionTracker.listen");
    }
}

std::chrono::milliseconds PositionTracker::getDuration(SegmentType segmentType, DurationType durationType, Timer::MotorState motorState) {
    std::chrono::milliseconds duration = std::chrono::milliseconds(0);
    if (motorState == Timer::MotorState::MOTOR_STOP) return duration;
    static const std::map<std::tuple<SegmentType, DurationType, Timer::MotorState>, std::chrono::milliseconds> combination_map = {
        { {SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_FAST}, DURATION_INGRESS_DISTANCE_VALID_FAST },
        { {SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_SLOW}, DURATION_INGRESS_DISTANCE_VALID_SLOW },
        { {SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, DURATION_INGRESS_EXPECTED_FAST },
        { {SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, DURATION_INGRESS_EXPECTED_SLOW },
        { {SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, DURATION_INGRESS_EXPIRED_FAST },
        { {SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, DURATION_INGRESS_EXPIRED_SLOW },

        { {SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, DURATION_HS_EXPECTED_FAST },
        { {SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, DURATION_HS_EXPECTED_SLOW },
        { {SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, DURATION_HS_EXPIRED_FAST },
        { {SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, DURATION_HS_EXPIRED_SLOW },

        { {SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_FAST}, DURATION_SORTING_DISTANCE_VALID_FAST },
        { {SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_SLOW}, DURATION_SORTING_DISTANCE_VALID_SLOW },
        { {SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, DURATION_SORTING_EXPECTED_FAST },
        { {SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, DURATION_SORTING_EXPECTED_SLOW },
        { {SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, DURATION_SORTING_EXPIRED_FAST },
        { {SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, DURATION_SORTING_EXPIRED_SLOW },

        { {SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, DURATION_EGRESS_EXPECTED_FAST },
        { {SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, DURATION_EGRESS_EXPECTED_SLOW },
        { {SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, DURATION_EGRESS_EXPIRED_FAST },
        { {SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, DURATION_EGRESS_EXPIRED_SLOW },
    };

    // Look up the combination and print the result
    auto it = combination_map.find(std::make_tuple(segmentType, durationType, motorState));
    if (it != combination_map.end()) {
        duration = it->second;
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to map SegmentType/DurationType/MotorState to duration", "PositionTracker.getDuration");
    }
    // Logger::getInstance().log(LogLevel::DEBUG, "Duration:" + std::to_string(duration.count()), "PositionTracker.getDuration");
    return duration;
}

uint32_t PositionTracker::nextPukId() {
    bool pukId = lastPukId.load();
    // TODO mutex for racecondition
    lastPukId++;
    return pukId;
}


void PositionTracker::onEvent(sc::rx::Observable<void>* event ,std::function<void()> callback) {
    event->subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver(callback)
        )
    );
}