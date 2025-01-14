/*
 * PositionTracker.cpp
 *
 *  Created on: 11.01.2025
 *      Author: Marc
 */

#include "headers/PositionTracker.h"
#include <map>
#include <tuple>

PositionTracker::PositionTracker(FSM* _fsm) {
    fsm = _fsm;
    listenThread = new std::thread(std::bind(&PositionTracker::listen, this));
    motorState1.store(Timer::MotorState::MOTOR_STOP);
    motorState2.store(Timer::MotorState::MOTOR_STOP);

    fsm->getFST_1_POSITION_INGRESS_NEW_PUK().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                uint32_t pukId = nextPukId();
                Puk* puk = new Puk(pukId);              
                std::lock_guard<std::mutex> lock(heightSensor1Mutex);
                heightSensor1.push(puk);

                Timer::MotorState motorState = motorState1.load();
                puk->approachingHS(
                    motorState,
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
                Logger::getInstance().log(LogLevel::DEBUG, "", "PositionTracker.onIngressNewPuk");
            })
        )
    );
    fsm->getFST_1_PUK_HEIGHT_IS_VALID().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Logger::getInstance().log(LogLevel::DEBUG, "", "PositionTracker.onIsValid");
            })
        )
    );
    fsm->getFST_1_PUK_HEIGHT_IS_NOT_VALID().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Logger::getInstance().log(LogLevel::DEBUG, "", "PositionTracker.onIsInvalid");
            })
        )
    );
    // TODO Puk that disappear or are sorted out
    fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_NEW_PUK().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Timer::MotorState motorState = motorState1.load();
                std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
                std::lock_guard<std::mutex> lockSorting(sorting1Mutex);
                Puk* puk = heightSensor1.front();
                heightSensor1.pop();
                sorting1.push(puk);

                puk->approachingSorting(
                    motorState,
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
                Logger::getInstance().log(LogLevel::DEBUG, "", "PositionTracker.onHSNewPuk");
            })
        )
    );
    fsm->getFST_1_PUK_IS_METAL().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Logger::getInstance().log(LogLevel::DEBUG, "", "PositionTracker.onIsMetal");
            })
        )
    );
    fsm->getFST_1_PUK_IS_NOT_METAL().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Logger::getInstance().log(LogLevel::DEBUG, "", "PositionTracker.onIsNotMetal");
            })
        )
    );
    fsm->getFST_1_POSITION_SORTING_NEW_PUK().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                Logger::getInstance().log(LogLevel::DEBUG, "", "PositionTracker.onSortingNewPuk");
            })
        )
    );
    

    
    
    fsm->getMOTOR_1_FAST().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                motorState1.store(Timer::MotorState::MOTOR_FAST);
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue egress1", "PositionTracker.onMotorFast");
                {
                    std::lock_guard<std::mutex> lock(egress1Mutex);
                    egress1 = updatePukQueue(egress1, Timer::MotorState::MOTOR_FAST);
                }
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue sorting1", "PositionTracker.onMotorFast");
                {
                    std::lock_guard<std::mutex> lock(sorting1Mutex);
                    sorting1 = updatePukQueue(sorting1, Timer::MotorState::MOTOR_FAST);
                }
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue heightSensor1", "PositionTracker.onMotorFast");
                {
                    std::lock_guard<std::mutex> lock(heightSensor1Mutex);
                    heightSensor1 = updatePukQueue(heightSensor1, Timer::MotorState::MOTOR_FAST);
                }
                Logger::getInstance().log(LogLevel::DEBUG, "MotorFast", "PositionTracker.onMotorFast");
            })
        )
    );
    fsm->getMOTOR_1_SLOW().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                motorState1.store(Timer::MotorState::MOTOR_SLOW);
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue egress1", "PositionTracker.onMotorSlow");
                {
                    std::lock_guard<std::mutex> lock(egress1Mutex);
                    egress1 = updatePukQueue(egress1, Timer::MotorState::MOTOR_SLOW);
                }
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue sorting1", "PositionTracker.onMotorSlow");
                {
                    std::lock_guard<std::mutex> lock(sorting1Mutex);
                    sorting1 = updatePukQueue(sorting1, Timer::MotorState::MOTOR_SLOW);
                }
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue heightSensor1", "PositionTracker.onMotorSlow");
                {
                    std::lock_guard<std::mutex> lock(heightSensor1Mutex);
                    heightSensor1 = updatePukQueue(heightSensor1, Timer::MotorState::MOTOR_SLOW);
                }
                // sorting1
                // egress1
                // for each (timer in timers1) { updateTimersToFast } 
                Logger::getInstance().log(LogLevel::DEBUG, "Motor Slow", "PositionTracker.onMotorSlow");
            })
        )
    );
    fsm->getMOTOR_1_STOP().subscribe(
        *new sc::rx::subscription<void>(
            *new PositionTrackerObserver([this](){
                motorState1.store(Timer::MotorState::MOTOR_STOP);
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue egress1", "PositionTracker.onMotorStop");
                {
                    std::lock_guard<std::mutex> lock(egress1Mutex);
                    egress1 = updatePukQueue(egress1, Timer::MotorState::MOTOR_STOP);
                }
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue sorting1", "PositionTracker.onMotorStop");
                {
                    std::lock_guard<std::mutex> lock(sorting1Mutex);
                    sorting1 = updatePukQueue(sorting1, Timer::MotorState::MOTOR_STOP);
                }
                Logger::getInstance().log(LogLevel::TRACE,  "Updating queue heightSensor1", "PositionTracker.onMotorStop");
                {
                    std::lock_guard<std::mutex> lock(heightSensor1Mutex);
                    heightSensor1 = updatePukQueue(heightSensor1, Timer::MotorState::MOTOR_STOP);
                }
                // for each (timer in timers1) { updateTimersToFast } 
                Logger::getInstance().log(LogLevel::DEBUG, "Motor Stop", "PositionTracker.onMotorStop");
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

std::queue<Puk*> PositionTracker::updatePukQueue(std::queue<Puk*> tempQueue, Timer::MotorState ms){
    std::queue<Puk*> updatedQueue;// = new std::queue<Puk*>();
    while (!tempQueue.empty()) {
        Puk* puk = tempQueue.front();
        Logger::getInstance().log(LogLevel::DEBUG,  "Processing Puk with ID: " + std::to_string(puk->getPukId()), "PositionTracker.updatePukQueue");
        puk->setTimers(ms);
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
            // TODO EGRESS1, Festo2.....
            default:
                pulseName = std::to_string(msg.code);
                break;
        }
        Logger::getInstance().log(LogLevel::DEBUG, "Message recieved\n\tcode: " + pulseName + "\n\tvalue: " + std::to_string(msg.value.sival_int), "PositionTracker.listen");
        // WAIT(1000);
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
        Logger::getInstance().log(LogLevel::DEBUG, "Uups", "PositionTracker.getDuration");
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