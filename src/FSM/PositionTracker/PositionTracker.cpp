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
        puk->setIsValid(true);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onIsValid");
    });
    onEvent(&fsm->getFST_1_PUK_HEIGHT_IS_NOT_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        Puk* puk = sorting1.front();
        puk->setIsValid(false); // FIXME add HS mocking in tests to make this testable
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onIsInvalid");
    });
    // TODO Puk that disappear or are sorted out
    onEvent(&fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = PositionTracker::queuePop(&heightSensor1);
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
        if (isMetalDesired1 && puk->getIsValid()) {
            // passthrough
            puk->setIsMetal(true);
            isMetalDesired1 = false;
            fsm->raiseFST_1_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Passing", "PositionTracker.onIsMetal");
        } else {
            // eject
            sorting1.pop();
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_1_PUK_SORTING_EJECT();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Ejecting", "PositionTracker.onIsMetal");
        }
    });
    onEvent(&fsm->getFST_1_PUK_IS_NOT_METAL(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = sorting1.front();
        if (!isMetalDesired1 && puk->getIsValid()) {
            // passthrough
            puk->setIsMetal(false);
            isMetalDesired1 = true;
            fsm->raiseFST_1_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Passing", "PositionTracker.onIsNotMetal");
        } else {
            // eject
            sorting1.pop();
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_1_PUK_SORTING_EJECT();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Ejecting", "PositionTracker.onIsNotMetal");
        }
    });
    onEvent(&fsm->getFST_1_POSITION_SORTING_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);
        std::lock_guard<std::mutex> lockEgress(egress1Mutex);

        Puk* puk = PositionTracker::queuePop(&sorting1);
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
        heightSensor2.push(puk);

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
        if (!puk->getIsValid()) {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Profile mismatch", "PositionTracker.onIsValid");
        } else {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIsValid");
        }
    });
    onEvent(&fsm->getFST_2_PUK_HEIGHT_IS_NOT_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        Puk* puk = heightSensor2.front();
        if (puk->getIsValid()) {
            // FIXME add HS mocking in tests to make this testable
            puk->setIsValid(false);
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Profile mismatch, invalidating puk", "PositionTracker.onIsInvalid");

        } {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIsInvalid");
        }
    });
    onEvent(&fsm->getFST_2_POSITION_HEIGHTMEASUREMENT_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = queuePop(&heightSensor2);
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
    onEvent(&fsm->getMOTOR_1_FAST(), [this](){
        motorState1.store(Timer::MotorState::MOTOR_FAST);
        {
            std::lock_guard<std::mutex> lock(egress1Mutex);
            egress1 = updatePukQueue(egress1, Timer::MotorState::MOTOR_FAST);
        }
        {
            std::lock_guard<std::mutex> lock(sorting1Mutex);
            sorting1 = updatePukQueue(sorting1, Timer::MotorState::MOTOR_FAST);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor1Mutex);
            heightSensor1 = updatePukQueue(heightSensor1, Timer::MotorState::MOTOR_FAST);
        }
        Logger::getInstance().log(LogLevel::TRACE, "[FST1] MotorFast", "PositionTracker.onMotorFast");
    });
    onEvent(&fsm->getMOTOR_1_SLOW(), [this](){
        motorState1.store(Timer::MotorState::MOTOR_SLOW);
        {
            std::lock_guard<std::mutex> lock(egress1Mutex);
            egress1 = updatePukQueue(egress1, Timer::MotorState::MOTOR_SLOW);
        }
        {
            std::lock_guard<std::mutex> lock(sorting1Mutex);
            sorting1 = updatePukQueue(sorting1, Timer::MotorState::MOTOR_SLOW);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor1Mutex);
            heightSensor1 = updatePukQueue(heightSensor1, Timer::MotorState::MOTOR_SLOW);
        }
        Logger::getInstance().log(LogLevel::TRACE, "[FST1] Motor Slow", "PositionTracker.onMotorSlow");
    });
    onEvent(&fsm->getMOTOR_1_STOP(), [this](){
        motorState1.store(Timer::MotorState::MOTOR_STOP);
        {
            std::lock_guard<std::mutex> lock(egress1Mutex);
            egress1 = updatePukQueue(egress1, Timer::MotorState::MOTOR_STOP);
        }
        {
            std::lock_guard<std::mutex> lock(sorting1Mutex);
            sorting1 = updatePukQueue(sorting1, Timer::MotorState::MOTOR_STOP);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor1Mutex);
            heightSensor1 = updatePukQueue(heightSensor1, Timer::MotorState::MOTOR_STOP);
        }
        // for each (timer in timers1) { updateTimersToFast } 
        Logger::getInstance().log(LogLevel::TRACE, "[FST1] Motor Stop", "PositionTracker.onMotorStop");
    });
    onEvent(&fsm->getMOTOR_2_FAST(), [this](){
        motorState2.store(Timer::MotorState::MOTOR_FAST);
        {
            std::lock_guard<std::mutex> lock(egress2Mutex);
            egress2 = updatePukQueue(egress2, Timer::MotorState::MOTOR_FAST);
        }
        {
            std::lock_guard<std::mutex> lock(sorting2Mutex);
            sorting2 = updatePukQueue(sorting2, Timer::MotorState::MOTOR_FAST);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor2Mutex);
            heightSensor2 = updatePukQueue(heightSensor2, Timer::MotorState::MOTOR_FAST);
        }
        Logger::getInstance().log(LogLevel::TRACE, "[FST2] MotorFast", "PositionTracker.onMotorFast");
    });
    onEvent(&fsm->getMOTOR_2_SLOW(), [this](){
        motorState2.store(Timer::MotorState::MOTOR_SLOW);
        {
            std::lock_guard<std::mutex> lock(egress2Mutex);
            egress2 = updatePukQueue(egress2, Timer::MotorState::MOTOR_SLOW);
        }
        {
            std::lock_guard<std::mutex> lock(sorting2Mutex);
            sorting2 = updatePukQueue(sorting2, Timer::MotorState::MOTOR_SLOW);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor2Mutex);
            heightSensor2 = updatePukQueue(heightSensor2, Timer::MotorState::MOTOR_SLOW);
        }
        Logger::getInstance().log(LogLevel::TRACE, "[FST2] Motor Slow", "PositionTracker.onMotorSlow");
    });
    onEvent(&fsm->getMOTOR_2_STOP(), [this](){
        motorState2.store(Timer::MotorState::MOTOR_STOP);
        {
            std::lock_guard<std::mutex> lock(egress2Mutex);
            egress2 = updatePukQueue(egress2, Timer::MotorState::MOTOR_STOP);
        }
        {
            std::lock_guard<std::mutex> lock(sorting2Mutex);
            sorting2 = updatePukQueue(sorting2, Timer::MotorState::MOTOR_STOP);
        }
        {
            std::lock_guard<std::mutex> lock(heightSensor2Mutex);
            heightSensor2 = updatePukQueue(heightSensor2, Timer::MotorState::MOTOR_STOP);
        }
        Logger::getInstance().log(LogLevel::TRACE, "[FST2] Motor Stop", "PositionTracker.onMotorStop");
    });
}

PositionTracker::~PositionTracker() {}

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
        return nullptr;
    }
    Puk* puk = queue->front();
    queue->pop();
    return puk;
}

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
                fsm->raiseFST_1_POSITION_DIVIDER_DISTANCE_VALID();
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
                fsm->raiseFST_2_POSITION_DIVIDER_DISTANCE_VALID();
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
        Logger::getInstance().log(LogLevel::TRACE, "Message recieved\n\tcode: " + pulseName + "\n\tvalue: " + std::to_string(msg.value.sival_int), "PositionTracker.listen");
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