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
    motorState1.store(MOTOR_STOP);
    motorState2.store(MOTOR_STOP);

    fsm->getFST_1_POSITION_INGRESS_NEW_PUK().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                
                
                uint32_t pukId = nextPukId();
                Puk * puk = new Puk(pukId);              
                heightSensor1.push(puk);
                puk->startNewValidTimer(*new Timer(connectionId, Timer::PulseCode::PULSE_INGRESS_1_DISTANCE_VALID, puk->getPukId(), calcDuration(Timer::PulseCode::PULSE_INGRESS_1_DISTANCE_VALID)));
                puk->startNewTimer(*new Timer(connectionId, Timer::PulseCode::PULSE_HS_1_PUK_EXPECTED, puk->getPukId(), calcDuration(Timer::PulseCode::PULSE_HS_1_PUK_EXPECTED)));
                puk->startNewTimer(*new Timer(connectionId, Timer::PulseCode::PULSE_HS_1_PUK_EXPIRED, puk->getPukId(), calcDuration(Timer::PulseCode::PULSE_HS_1_PUK_EXPIRED)));

                Logger::getInstance().log(LogLevel::DEBUG, "PositionTrackerObserver.next was called \n\tpukId: " + std::to_string(puk->getPukId()) + "\n\ttrigger: INGRESS_NEW_PUK", "PositionTrackerObserver");
            })
        )
    );
    // TODO fsm->getFST_1_PUK_HEIGHT_IS_VALID()
    // TODO fsm->getFST_1_PUK_HEIGHT_IS_NOT_VALID()
    fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_NEW_PUK().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Puk* puk = heightSensor1.front();
                heightSensor1.pop();
                puk->deleteListTimers();
                sorting1.push(puk);
                puk->startNewTimer(*new Timer(connectionId, Timer::PulseCode::PULSE_SORTING_1_PUK_EXPECTED, puk->getPukId(), calcDuration(Timer::PulseCode::PULSE_SORTING_1_PUK_EXPECTED)));
                puk->startNewTimer(*new Timer(connectionId, Timer::PulseCode::PULSE_SORTING_1_PUK_EXPIRED, puk->getPukId(), calcDuration(Timer::PulseCode::PULSE_SORTING_1_PUK_EXPIRED)));
                Logger::getInstance().log(LogLevel::DEBUG, "PositionTrackerObserver.next was called \n\tpukId: " + std::to_string(puk->getPukId()) + "\n\ttrigger: HEIGHTMEASUREMENT_NEW_PUK", "PositionTrackerObserver");
            })
        )
    );

    
    
    fsm->getMOTOR_1_FAST().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                motorState1.store(MOTOR_FAST);
                // std::queue<Puk*> tempQueue = heightSensor1;
                // // Iterate over the temporary queue
                // while (!tempQueue.empty()) {
                //     Puk* puk = tempQueue.front();
                //     std::cout << "Processing Puk with ID: " << puk->id << std::endl;
                //     tempQueue.pop();  // Remove the element from the temporary queue
                // }
                // sorting1
                // egress1


                // for each (timer in timers1) { updateTimersToFast } 
                Logger::getInstance().log(LogLevel::DEBUG, "MOTOR FAST", "PositionTrackerObserver");
            })
        )
    );
    fsm->getMOTOR_1_SLOW().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                motorState1.store(MOTOR_SLOW);
                Logger::getInstance().log(LogLevel::DEBUG,  "Updating queue egress1", "PositionTracker.onMotorSlow");
                egress1 = updatePukQueue(egress1, MOTOR_SLOW);
                Logger::getInstance().log(LogLevel::DEBUG,  "Updating queue sorting1", "PositionTracker.onMotorSlow");
                sorting1 = updatePukQueue(sorting1, MOTOR_SLOW);
                Logger::getInstance().log(LogLevel::DEBUG,  "Updating queue heightSensor1", "PositionTracker.onMotorSlow");
                heightSensor1 = updatePukQueue(heightSensor1, MOTOR_SLOW);
                // sorting1
                // egress1
                // for each (timer in timers1) { updateTimersToFast } 
                Logger::getInstance().log(LogLevel::DEBUG, "MOTOR SLOW", "PositionTrackerObserver");
            })
        )
    );
    fsm->getMOTOR_1_STOP().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                // motorState1.store(MOTOR_STOP);
                // for each (timer in timers1) { updateTimersToFast } 
                // Logger::getInstance().log(LogLevel::DEBUG, "PositionTrackerObserver.next was called \n\tpukId: " + std::to_string(pukId), "PositionTrackerObserver");
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

}

std::queue<Puk*> PositionTracker::updatePukQueue(std::queue<Puk*> tempQueue, MotorState ms){
    std::queue<Puk*> updatedQueue;// = new std::queue<Puk*>();
    while (!tempQueue.empty()) {
        Puk* puk = tempQueue.front();
        switch (ms) {
            case MOTOR_FAST:
                puk->updateTimersFast();
                break;
            case MOTOR_SLOW:
                Logger::getInstance().log(LogLevel::DEBUG,  "Motorslow", "PositionTracker.updatePukQueue");
                puk->updateTimersSlow();
                break;
            case MOTOR_STOP:
                puk->updateTimersStop();
                break;
            default:
                Logger::getInstance().log(LogLevel::ERROR,  "OhOh", "PositionTracker.updatePukQueue");
        }
        Logger::getInstance().log(LogLevel::DEBUG,  "Processing Puk with ID: " + std::to_string(puk->getPukId()), "PositionTracker.updatePukQueue");
        tempQueue.pop();  // Remove the element from the temporary queue
        updatedQueue.push(puk);
    }

    return updatedQueue;
}


PositionTracker::~PositionTracker() {}

void PositionTracker::listen() {
    // Connection ID für Pulse Message
    uint32_t channelId = ChannelCreate(0);
    connectionId = ConnectAttach(0, 0, channelId, 0, 0); // Fehlerbehandlung fehlt
    _pulse msg;

    while (1) {
        // Logger::getInstance().log(LogLevel::DEBUG, "Listening loop", "PositionTracker.listen");
        MsgReceive(channelId, &msg, sizeof(msg), NULL);
        switch (msg.code) {
            case Timer::PulseCode::PULSE_INGRESS_1_DISTANCE_VALID:
                fsm->raiseFST_1_POSITION_INGRESS_DISTANCE_VALID();
                Logger::getInstance().log(LogLevel::DEBUG, "Message recieved\n\tcode: PULSE_INGRESS_DISTANCE_VALID\n\tvalue: " + std::to_string(msg.value.sival_int), "PositionTracker.listen");
                break;
            case Timer::PulseCode::PULSE_HS_1_PUK_EXPECTED:
                Logger::getInstance().log(LogLevel::DEBUG, "Message recieved\n\tcode: PULSE_HS_PUK_EXPECTED\n\tvalue: " + std::to_string(msg.value.sival_int), "PositionTracker.listen");
                fsm->raiseFST_1_POSITION_HEIGHTMEASUREMENT_PUK_EXPECTED();
                break;
            case Timer::PulseCode::PULSE_HS_1_PUK_EXPIRED:
                Logger::getInstance().log(LogLevel::DEBUG, "Message recieved\n\tcode: PULSE_HS_PUK_EXPIRED\n\tvalue: " + std::to_string(msg.value.sival_int), "PositionTracker.listen");
                fsm->raiseFST_1_POSITION_HEIGHTMEASUREMENT_PUK_EXPIRED();
                break;
            default:
                Logger::getInstance().log(LogLevel::DEBUG, "Message recieved\n\tcode: " + std::to_string(msg.code) + "\n\tvalue: " + std::to_string(msg.value.sival_int), "PositionTracker.listen");
        }
        // WAIT(1000);
    }
}

std::chrono::milliseconds PositionTracker::calcDuration(Timer::PulseCode code) {
    MotorState ms1 = motorState1.load();
    MotorState ms2 = motorState2.load();
    std::chrono::milliseconds duration = std::chrono::milliseconds(0);
    switch (code) {
        case Timer::PulseCode::PULSE_INGRESS_1_DISTANCE_VALID:
            if (ms1 == MOTOR_STOP) return duration;
            duration = ms1 == MOTOR_FAST ? DURATION_INGRESS_DISTANCE_VALID_FAST : DURATION_INGRESS_DISTANCE_VALID_SLOW;
            break;
        case Timer::PulseCode::PULSE_HS_1_PUK_EXPECTED:
            if (ms1 == MOTOR_STOP) return duration;
            duration = ms1 == MOTOR_FAST ? DURATION_HS_PUK_EXPECTED_FAST : DURATION_HS_PUK_EXPECTED_SLOW;
            break;
        case Timer::PulseCode::PULSE_HS_1_PUK_EXPIRED:
            if (ms1 == MOTOR_STOP) return duration;
            duration = ms1 == MOTOR_FAST ? DURATION_HS_PUK_EXPIRED_FAST : DURATION_HS_PUK_EXPIRED_SLOW;
            break;
        case Timer::PulseCode::PULSE_SORTING_1_PUK_EXPECTED:
            if (ms1 == MOTOR_STOP) return duration;
            duration = ms1 == MOTOR_FAST ? DURATION_SORTING_PUK_EXPECTED_FAST : DURATION_SORTING_PUK_EXPECTED_SLOW;
            break;
        case Timer::PulseCode::PULSE_SORTING_1_PUK_EXPIRED:
            if (ms1 == MOTOR_STOP) return duration;
            duration = ms1 == MOTOR_FAST ? DURATION_SORTING_PUK_EXPIRED_FAST : DURATION_SORTING_PUK_EXPIRED_SLOW;
            break;
        default:
            Logger::getInstance().log(LogLevel::ERROR, "Unknown PulseCode...", "PositionTracker.calcDuration");
    }
    Logger::getInstance().log(LogLevel::DEBUG, "Duration:" + std::to_string(duration.count()), "PositionTracker.calcDuration");
    return duration;
}

uint32_t PositionTracker::nextPukId() {
    bool pukId = lastPukId.load();
    // TODO mutex for racecondition
    lastPukId++;
    return pukId;
}