/*
 * PositionTracker.cpp
 *
 *  Created on: 11.01.2025
 *      Author: Marc
 */

#include "headers/PositionTracker.h"
// constexpr std::chrono::milliseconds Duration::Ingress::Expected::Fast;
// constexpr std::chrono::milliseconds Duration::Ingress::Expected::Slow;
// constexpr std::chrono::milliseconds Duration::Ingress::Expired::Fast;
// constexpr std::chrono::milliseconds Duration::Ingress::Expired::Slow;
// constexpr std::chrono::milliseconds Duration::Ingress::Mean::Fast;
// constexpr std::chrono::milliseconds Duration::Ingress::Mean::Slow;
// constexpr std::chrono::milliseconds Duration::Ingress::DistanceValid::Fast;
// constexpr std::chrono::milliseconds Duration::Ingress::DistanceValid::Slow;

// constexpr std::chrono::milliseconds Duration::HeightSensor::Expected::Fast;
// constexpr std::chrono::milliseconds Duration::HeightSensor::Expected::Slow;
// constexpr std::chrono::milliseconds Duration::HeightSensor::Expired::Fast;
// constexpr std::chrono::milliseconds Duration::HeightSensor::Expired::Slow;
// constexpr std::chrono::milliseconds Duration::HeightSensor::Mean::Fast;
// constexpr std::chrono::milliseconds Duration::HeightSensor::Mean::Slow;

// constexpr std::chrono::milliseconds Duration::Sorting::Expected::Fast;
// constexpr std::chrono::milliseconds Duration::Sorting::Expected::Slow;
// constexpr std::chrono::milliseconds Duration::Sorting::Expired::Fast;
// constexpr std::chrono::milliseconds Duration::Sorting::Expired::Slow;
// constexpr std::chrono::milliseconds Duration::Sorting::Mean::Fast;
// constexpr std::chrono::milliseconds Duration::Sorting::Mean::Slow;
// constexpr std::chrono::milliseconds Duration::Sorting::DistanceValid::Fast;
// constexpr std::chrono::milliseconds Duration::Sorting::DistanceValid::Slow;

// constexpr std::chrono::milliseconds Duration::Egress::Expected::Fast;
// constexpr std::chrono::milliseconds Duration::Egress::Expected::Slow;
// constexpr std::chrono::milliseconds Duration::Egress::Expired::Fast;
// constexpr std::chrono::milliseconds Duration::Egress::Expired::Slow;
// constexpr std::chrono::milliseconds Duration::Egress::Mean::Fast;
// constexpr std::chrono::milliseconds Duration::Egress::Mean::Slow;

PositionTracker::PositionTracker(FSM* _fsm) {
    fsm = _fsm;
    listenThread = new std::thread(std::bind(&PositionTracker::listen, this));
    motorState1.store(Timer::MotorState::MOTOR_STOP);
    motorState2.store(Timer::MotorState::MOTOR_STOP);
    // TODO CALCULATE THESE!!! -> is at the bottom row 942!!

    // durations.fst1.ingress.distanceValid.Fast = std::chrono::milliseconds(100);
    // durations.fst1.ingress.distanceValid.Slow = std::chrono::milliseconds(200);
    
    // // durations.fst1.sorting.distanceValid.Fast = std::chrono::milliseconds(1500);
    // // durations.fst1.sorting.distanceValid.Slow = std::chrono::milliseconds(3000);
    
    // // durations.fst2.ingress.distanceValid.Fast = std::chrono::milliseconds(100);
    // // durations.fst2.ingress.distanceValid.Slow = std::chrono::milliseconds(200);
    
    // durations.fst2.sorting.distanceValid.Fast = std::chrono::milliseconds(1500); // we dont need it or? DD
    // durations.fst2.sorting.distanceValid.Slow = std::chrono::milliseconds(3000); // we dont need it or? DD

    // ---fake calibration---
    //     durations.fst1.ingress.distanceValid.Fast:835ms
    // durations.fst1.ingress.distanceValid.Slow:2990ms
    // durations.fst1.ingress.expected.Fast:1670ms
    // durations.fst1.ingress.expected.Slow:5980ms
    // durations.fst1.ingress.expired.Fast:2551ms
    // durations.fst1.ingress.expired.Slow:7650ms
    // durations.fst1.heightSensor.expected.Fast:617ms
    // durations.fst1.heightSensor.expected.Slow:2905ms
    // durations.fst1.heightSensor.expired.Fast:1183ms
    // durations.fst1.heightSensor.expired.Slow:3388ms
    // durations.fst1.sorting.distanceValid.Fast:734ms
    // durations.fst1.sorting.distanceValid.Slow:2773ms
    // durations.fst1.sorting.expected.Fast:1468ms
    // durations.fst1.sorting.expected.Slow:5547ms
    // durations.fst1.sorting.expired.Fast:2014ms
    // durations.fst1.sorting.expired.Slow:5951ms
    // durations.fst1.egress.expected.Fast:50ms
    // durations.fst1.egress.expected.Slow:100ms
    // durations.fst1.egress.expired.Fast:2000ms
    // durations.fst1.egress.expired.Slow:4000ms
    // durations.fst2.ingress.distanceValid.Fast:782ms
    // durations.fst2.ingress.distanceValid.Slow:2871ms
    // durations.fst2.ingress.expected.Fast:1565ms
    // durations.fst2.ingress.expected.Slow:5742ms
    // durations.fst2.ingress.expired.Fast:2136ms
    // durations.fst2.ingress.expired.Slow:6278ms
    // durations.fst2.heightSensor.expected.Fast:648ms
    // durations.fst2.heightSensor.expected.Slow:2988ms
    // durations.fst2.heightSensor.expired.Fast:1218ms
    // durations.fst2.heightSensor.expired.Slow:3565ms
    // durations.fst2.sorting.distanceValid.Fast:600ms
    // durations.fst2.sorting.distanceValid.Slow:2306ms
    // durations.fst2.sorting.expected.Fast:1201ms
    // durations.fst2.sorting.expected.Slow:4613ms
    // durations.fst2.sorting.expired.Fast:1777ms
    // durations.fst2.sorting.expired.Slow:5219ms
    durations.fst1.ingress.distanceValid.Fast = std::chrono::milliseconds(807);
    durations.fst1.ingress.distanceValid.Slow = std::chrono::milliseconds(2993);
    durations.fst1.ingress.expected.Fast = std::chrono::milliseconds(1615);
    durations.fst1.ingress.expected.Slow = std::chrono::milliseconds(5986);
    durations.fst1.ingress.expired.Fast = std::chrono::milliseconds(2632);
    durations.fst1.ingress.expired.Slow = std::chrono::milliseconds(7896);
    durations.fst1.heightSensor.expected.Fast = std::chrono::milliseconds(688);
    durations.fst1.heightSensor.expected.Slow = std::chrono::milliseconds(2983);
    durations.fst1.heightSensor.expired.Fast = std::chrono::milliseconds(1228);
    durations.fst1.heightSensor.expired.Slow = std::chrono::milliseconds(3530);
    durations.fst1.sorting.distanceValid.Fast = std::chrono::milliseconds(668);
    durations.fst1.sorting.distanceValid.Slow = std::chrono::milliseconds(2531);
    durations.fst1.sorting.expected.Fast = std::chrono::milliseconds(1337);
    durations.fst1.sorting.expected.Slow = std::chrono::milliseconds(5062);
    durations.fst1.sorting.expired.Fast = std::chrono::milliseconds(1930);
    durations.fst1.sorting.expired.Slow = std::chrono::milliseconds(5726);
    durations.fst1.egress.expected.Fast = std::chrono::milliseconds(50);
    durations.fst1.egress.expected.Slow = std::chrono::milliseconds(100);
    durations.fst1.egress.expired.Fast = std::chrono::milliseconds(2000);
    durations.fst1.egress.expired.Slow = std::chrono::milliseconds(4000);
    durations.fst2.ingress.distanceValid.Fast = std::chrono::milliseconds(739);
    durations.fst2.ingress.distanceValid.Slow = std::chrono::milliseconds(2773);
    durations.fst2.ingress.expected.Fast = std::chrono::milliseconds(1478);
    durations.fst2.ingress.expected.Slow = std::chrono::milliseconds(5547);
    durations.fst2.ingress.expired.Fast = std::chrono::milliseconds(2068);
    durations.fst2.ingress.expired.Slow = std::chrono::milliseconds(6125);
    durations.fst2.heightSensor.expected.Fast = std::chrono::milliseconds(670);
    durations.fst2.heightSensor.expected.Slow = std::chrono::milliseconds(2939);
    durations.fst2.heightSensor.expired.Fast = std::chrono::milliseconds(1210);
    durations.fst2.heightSensor.expired.Slow = std::chrono::milliseconds(3540);
    durations.fst2.sorting.distanceValid.Fast = std::chrono::milliseconds(590);
    durations.fst2.sorting.distanceValid.Slow = std::chrono::milliseconds(2279);
    durations.fst2.sorting.expected.Fast = std::chrono::milliseconds(1181);
    durations.fst2.sorting.expected.Slow = std::chrono::milliseconds(4558);
    durations.fst2.sorting.expired.Fast = std::chrono::milliseconds(1757);
    durations.fst2.sorting.expired.Slow = std::chrono::milliseconds(5149);
    fsm->setDigitpermm(58);
    fsm->setDigitpermm2(53);
    fsm->setCalibrated(true);
    // ----------------------------
    
    onEvent(&fsm->getFST_1_POSITION_INGRESS_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lock(heightSensor1Mutex);
        // Logger::getInstance().log(LogLevel::DEBUG, "[FST1] " + 
        //     std::to_string(heightSensor1.size()) +
        //     std::to_string(sorting1.size()) +
        //     std::to_string(egress1.size()) +
        //     std::to_string(ingress2.size()) +
        //     std::to_string(heightSensor2.size()) +
        //     std::to_string(sorting2.size())+ 
        //     std::to_string(egress2.size()), 
        //     "PositionTracker.onIngressNewPuk");
        Puk* puk = new Puk(nextPukId());              
        heightSensor1.push(puk);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] PUK between LBF1 and HS1 - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                    " PUK Type: " + puk->pukTypeToString(puk->getPukType()) +
                                                    " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()), " PositionTracker");
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size())+ 
                std::to_string(egress2.size()), 
                "PositionTracker.onIngressNewPuk");
            return;
        }
        puk->approachingHS(
            motorState1.load(),
            new Timer(
                getDuration(FESTO1, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_SLOW), 
                connectionId, 
                Timer::PulseCode::PULSE_INGRESS_1_DISTANCE_VALID, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO1, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW), 
                connectionId, 
                Timer::PulseCode::PULSE_HS_1_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO1, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW), 
                connectionId, 
                Timer::PulseCode::PULSE_HS_1_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onIngressNewPuk");
    });
    onEvent(&fsm->getFST_1_PUK_HEIGHT_IS_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(sorting1Mutex);
        Puk* puk = sorting1.front();  // FST1 raises "puk valid/inValid" after "sorting new puk", so we need to use sorting1.front() here
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIngressNewPuk");
            return;
        }
        puk->setIsValid(true);
        puk->setPukType(Puk::PUKType::PUK_WITH_HOLE);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] HEIGHT_IS_VALID", "PositionTracker.onIsValid");
    });
    onEvent(&fsm->getFST_1_PUK_HEIGHT_IS_NOT_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(sorting1Mutex);
        Puk* puk = sorting1.front();
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIsNotValid");
            return;
        }
        puk->setIsValid(false); // FIXME add HS mocking in tests to make this testable
        puk->setPukType(Puk::PUKType::PUK_WITHOUT_HOLE);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] HEIGHT_IS_NOT_VALID", "PositionTracker.onIsNotValid");
    });
    onEvent(&fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor1Mutex);
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = PositionTracker::queuePop(&heightSensor1);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] nullpointer exception", "PositionTracker.onHSNewPuk");
            return;
        }
        sorting1.push(puk);
        int32_t averageHeight = fsm->getAverageHeight();
        puk->setAverageHeight(averageHeight);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] PUK between HS1 and MS1 - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                    " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                    " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()), "PositionTracker");

        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onHSNewPuk");
            return;
        }
        puk->approachingSorting(
            motorState1.load(),
            new Timer(
                getDuration(FESTO1, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_1_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO1, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
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
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIsMetal");
            return;
        }
        puk->setPukType(Puk::PUKType::PUK_WITH_HOLE_AND_METAL);

        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] currentSortingType1: " +std::to_string(currentSortingType1) + " puk->getIsValid() = " + std::to_string(puk->getIsValid()) , "PositionTracker.onIsMetal");
        if (currentSortingType1 != PUK_B && puk->getIsValid()) {
            // passthrough
            puk->setIsMetal(true);
            currentSortingType1 = sortingTypeNext(currentSortingType1, FESTO1);
            fsm->raiseFST_1_PUK_SORTING_PASSTHROUGH();
            //Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Passing", "PositionTracker.onIsMetal");
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] PUK Passthrough - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        "PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()), "PositionTracker.onIsMetal");

        } else {
            // eject
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_1_PUK_SORTING_EJECT();
            //Logger::getInstance().log(LogLevel::DEBUG, "[FST1] Ejecting", "PositionTracker.onIsMetal");
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] PUK Ejected - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        "PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()) +  
                                                        " PUK desired(A=0,B=1,C=2): A oder C", "PositionTracker.onIsMetal");

        }
    });
    onEvent(&fsm->getFST_1_PUK_IS_NOT_METAL(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);

        Puk* puk = sorting1.front();

        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIsNotMetal");
            return;
        }

        puk->setPukType(Puk::PUKType::PUK_WITH_HOLE);

        
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] currentSortingType1: " +std::to_string(currentSortingType1) + " puk->getIsValid() = " + std::to_string(puk->getIsValid()) , "PositionTracker.onIsNotMetal");
        if (currentSortingType1 == PUK_B && puk->getIsValid()) {
            // passthrough
            puk->setIsMetal(false);
            currentSortingType1 = sortingTypeNext(currentSortingType1, FESTO1);
            fsm->raiseFST_1_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] PUK Passthroug - PUK-ID: " + std::to_string(puk->getPukId()) + 
                                                        " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()), "PositionTracker.onIsNotMetal");
        } else {
            // eject
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_1_PUK_SORTING_EJECT();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST1] PUK Ejected - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()) +
                                                        " PUK desired(A=0,B=1,C=2): B", "PositionTracker.onIsNotMetal");
        }
    });
    onEvent(&fsm->getFST_1_POSITION_SORTING_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting1Mutex);
        std::lock_guard<std::mutex> lockEgress(egress1Mutex);

        // Logger::getInstance().log(LogLevel::DEBUG, "[FST1] " + 
        //     std::to_string(heightSensor1.size()) +
        //     std::to_string(sorting1.size()) +
        //     std::to_string(egress1.size()) +
        //     std::to_string(ingress2.size()) +
        //     std::to_string(heightSensor2.size()) +
        //     std::to_string(sorting2.size()) +
        //     std::to_string(egress2.size()), 
        //     "PositionTracker.onSortingNewPuk");
        Puk* puk = PositionTracker::queuePop(&sorting1);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onSortingNewPuk");
            return;
        }
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1] PUK between LBM_1 and LBE_1 - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                    " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                    " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()), "PositionTracker");
        egress1.push(puk);

        puk->approachingEgress(
            motorState1.load(),
            new Timer(
                getDuration(FESTO1, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_FAST),
                getDuration(FESTO1, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_1_DISTANCE_VALID, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO1, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_1_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO1, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_1_PUK_EXPIRED, 
                puk->getPukId()
            ),
            nullptr
            // new Timer(
            //     TIMEOUT_DIVERTER,
            //     connectionId,
            //     Timer::PulseCode::PULSE_SORTING_1_TIMEOUT_DIVERTER,
            //     puk->getPukId()
            // )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1]", "PositionTracker.onSortingNewPuk");
    });
    onEvent(&fsm->getFST_1_SORTING_MODULE_ACTIVE(), [this]() {
        diverter1TimeOut = new Timer(
            TIMEOUT_DIVERTER,
            connectionId,
            Timer::PulseCode::PULSE_SORTING_1_TIMEOUT_DIVERTER,
            0
        );
        diverter1TimeOut->setMotorState(Timer::MotorState::MOTOR_FAST);
    });
    onEvent(&fsm->getFST_1_SORTING_MODULE_RESTING(), [this]() {
        if (diverter1TimeOut == nullptr) {
            Logger::getInstance().log(LogLevel::WARNING, "[FST1]", "PositionTracker.onSortingModuleResting");
        } else {
            diverter1TimeOut->kill();
        }
    });
    onEvent(&fsm->getFST_1_POSITION_EGRESS_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockEgress(egress1Mutex);
        std::lock_guard<std::mutex> lockSorting(ingress2Mutex);

        // Logger::getInstance().log(LogLevel::DEBUG, "[FST1] " + 
        //     std::to_string(heightSensor1.size()) +
        //     std::to_string(sorting1.size()) +
        //     std::to_string(egress1.size()) +
        //     std::to_string(ingress2.size()) +
        //     std::to_string(heightSensor2.size()) +
        //     std::to_string(sorting2.size()) +
        //     std::to_string(egress2.size()), 
        //     "PositionTracker.onEgressNewPuk");
        Puk* puk = queuePop(&egress1);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onEgressNewPuk");
            return;
        }
        ingress2.push(puk);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST1&2] PUK between LBE1 and LBF2 - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                    "PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                    "PUK AverageHeight: " + std::to_string(puk->getAverageHeight()), "PositionTracker");

        puk->approachingIngress(
            motorState2.load(), // motorstate1?
            new Timer(
                getDuration(FESTO1, SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_INGRESS_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO1, SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO1, SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
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

        // Logger::getInstance().log(LogLevel::DEBUG, "[FST2] " + 
        //     std::to_string(heightSensor1.size()) +
        //     std::to_string(sorting1.size()) +
        //     std::to_string(egress1.size()) +
        //     std::to_string(ingress2.size()) +
        //     std::to_string(heightSensor2.size()) +
        //     std::to_string(sorting2.size()) +
        //     std::to_string(egress2.size()), 
        //     "PositionTracker.onIngressNewPuk");
        Puk* puk = queuePop(&ingress2);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIngressNewPuk");
            return;
        }
        heightSensor2.push(puk);
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2] PUK between LBF_2 and HS_2 - PUK-ID: " + std::to_string(puk->getPukId()) + 
                                                    " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                    " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()), "PositionTracker");

        puk->approachingHS(
            motorState2.load(),
            new Timer(
                getDuration(FESTO2, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO2, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_SLOW), 
                connectionId, 
                Timer::PulseCode::PULSE_NOOP, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO2, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO2, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_HS_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO2, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO2, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_HS_2_PUK_EXPIRED, 
                puk->getPukId()
            )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIngressNewPuk");
    });
    onEvent(&fsm->getFST_2_PUK_HEIGHT_IS_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(sorting2Mutex);
        Puk* puk = sorting2.front();
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIsValid");
            return;
        }
        if (!puk->getIsValid()) {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Profile mismatch", "PositionTracker.onIs_NOT_Valid"); //Why Pofile missmatch?
        } else {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onIsValid");
        }
    });
    onEvent(&fsm->getFST_2_PUK_HEIGHT_IS_NOT_VALID(), [this](){
        std::lock_guard<std::mutex> lockHeightSensor(sorting2Mutex);
        Puk* puk = sorting2.front();
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIsInvalid");
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
        std::lock_guard<std::mutex> lockHeightSensor(heightSensor2Mutex);
        std::lock_guard<std::mutex> lockSorting(sorting2Mutex);

        Puk* puk = queuePop(&heightSensor2);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onHSNewPuk");
            return;
        }
        sorting2.push(puk);       
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2] PUK between HS2 and MS2 - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                    " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) +
                                                    " PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()) +
                                                    " PUK AverageHeight FST2: " + std::to_string(fsm->getAverageHeight2()), "PositionTracker");
        puk->approachingSorting(
            motorState2.load(),
            new Timer(
                getDuration(FESTO2, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO2, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO2, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO2, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
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
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIsMetal");
            return;
        }

        Logger::getInstance().log(LogLevel::DEBUG, "[FST2] currentSortingType2: " + std::to_string(currentSortingType2) + 
            " puk->getIsValid() = " + std::to_string(puk->getIsValid()) +
            " averageHeights 1/2/threshhold " + 
            std::to_string(puk->getAverageHeight()) + "/" + 
            std::to_string(fsm->getAverageHeight2()) + "/" + 
            std::to_string(HEIGHTTHRESHHOLD), "PositionTracker.onIsNotMetal");
        if (currentSortingType2 != PUK_B && puk->getIsMetal() && puk->getIsValid() && (abs(puk->getAverageHeight() - fsm->getAverageHeight2()) <= HEIGHTTHRESHHOLD ) ) {
            // passthrough
            currentSortingType2 = sortingTypeNext(currentSortingType2, FESTO2);
            fsm->raiseFST_2_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] PUK Passthrough - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight: " + std::to_string(puk->getAverageHeight()) +
                                                        " PUK Average Height FST2: " + std::to_string(fsm->getAverageHeight2()) +
                                                        " PUK desired(A=0,B=1,C=2): " + std::to_string(currentSortingType2), "PositionTracker.onIsMetal");

        } else {
            // eject
            puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
            fsm->raiseFST_2_PUK_SORTING_EJECT();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] PUK Ejecting - PUK-ID: " + std::to_string(puk->getPukId()) + 
                                                        " PUK Type " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()) + 
                                                        " PUK Average Height FST2: " + std::to_string(fsm->getAverageHeight2()) +
                                                        " PUK desired(A=0,B=1,C=2): " + std::to_string(currentSortingType2), "PositionTracker.onIsMetal");
        }
    });
    onEvent(&fsm->getFST_2_PUK_IS_NOT_METAL(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting2Mutex);

        Puk* puk = sorting2.front();
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onIsMetal");
            return;
        }

        Logger::getInstance().log(LogLevel::DEBUG, "[FST2] currentSortingType2: " + std::to_string(currentSortingType2) + 
            " puk->getIsValid() = " + std::to_string(puk->getIsValid()) +
            " averageHeights 1/2/threshhold " + 
            std::to_string(puk->getAverageHeight()) + "/" + 
            std::to_string(fsm->getAverageHeight2()) + "/" + 
            std::to_string(HEIGHTTHRESHHOLD), "PositionTracker.onIsNotMetal");
        if (currentSortingType2 == PUK_B && !puk->getIsMetal() && puk->getIsValid() && (abs(puk->getAverageHeight() - fsm->getAverageHeight2()) <= HEIGHTTHRESHHOLD )) {
            // passthrough
            currentSortingType2 = sortingTypeNext(currentSortingType2, FESTO2);
            fsm->raiseFST_2_PUK_SORTING_PASSTHROUGH();
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] PUK Passthrough - PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()) +
                                                        " PUK AverageHeight FST2: " + std::to_string(fsm->getAverageHeight2()), "PositionTracker.onIsNotMetal");
            return;
        }

        // eject
        if (puk->getIsMetal()) {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] PUK Ejecting - PUK-ID: " + std::to_string(puk->getPukId()) + 
                                                        " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight())  + 
                                                        " PUK Average Height FST2: " + std::to_string(fsm->getAverageHeight2()) +
                                                        " PUK desired(A=0,B=1,C=2): A oder C", "PositionTracker");
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] EjectionReason: Metal mismatch", "PositionTracker.onIsNotMetal");
        } else {
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Ejecting PUK TYPE: " + std::to_string(currentSortingType2), "PosiTionTracher.OnIs_NOT_METAL");
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] PUK Ejecting - PUK-ID: " + std::to_string(puk->getPukId()) + 
                                                        " PUK Type: " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()) + 
                                                        " PUK Average Height FST2: " +std::to_string(fsm->getAverageHeight2()) +
                                                        " PUK desired(A=0,B=1,C=2): A oder C", "PositionTracker");
            Logger::getInstance().log(LogLevel::DEBUG, "[FST2] Ejecting", "PositionTracker.onIsNotMetal");
        }
        puk->setTimers(Timer::MotorState::MOTOR_STOP); // could also implement puk->killTimers()
        fsm->raiseFST_2_PUK_SORTING_EJECT();
    });
    onEvent(&fsm->getFST_2_POSITION_SORTING_NEW_PUK(), [this](){
        std::lock_guard<std::mutex> lockSorting(sorting2Mutex);
        std::lock_guard<std::mutex> lockHeightSensor(egress2Mutex);

        Puk* puk = queuePop(&sorting2);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] " + 
                std::to_string(heightSensor1.size()) +
                std::to_string(sorting1.size()) +
                std::to_string(egress1.size()) +
                std::to_string(ingress2.size()) +
                std::to_string(heightSensor2.size()) +
                std::to_string(sorting2.size()) +
                std::to_string(egress2.size()), 
                "PositionTracker.onSortingNewPuk");
            return;
        }
        egress2.push(puk);


        puk->approachingEgress(
            motorState2.load(),
            new Timer(
                getDuration(FESTO2, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_FAST),
                getDuration(FESTO2, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_SORTING_2_DISTANCE_VALID, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO2, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO2, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_2_PUK_EXPECTED, 
                puk->getPukId()
            ),
            new Timer(
                getDuration(FESTO2, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST), 
                getDuration(FESTO2, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW),
                connectionId, 
                Timer::PulseCode::PULSE_EGRESS_2_PUK_EXPIRED, 
                puk->getPukId()
            ),
            nullptr
            // new Timer(
            //     TIMEOUT_DIVERTER,
            //     connectionId,
            //     Timer::PulseCode::PULSE_SORTING_2_TIMEOUT_DIVERTER,
            //     puk->getPukId()
            // )
        );
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2]", "PositionTracker.onSortingNewPuk");
    });
    onEvent(&fsm->getFST_2_SORTING_MODULE_ACTIVE(), [this]() {
        diverter2TimeOut = new Timer(
            TIMEOUT_DIVERTER,
            connectionId,
            Timer::PulseCode::PULSE_SORTING_2_TIMEOUT_DIVERTER,
            0
        );
        
        diverter2TimeOut->setMotorState(Timer::MotorState::MOTOR_FAST);
    });
    onEvent(&fsm->getFST_2_SORTING_MODULE_RESTING(), [this]() {
        if (diverter2TimeOut == nullptr) {
            Logger::getInstance().log(LogLevel::WARNING, "[FST2]", "PositionTracker.onSortingModuleResting");
        } else {
            diverter2TimeOut->kill();
        }
    });
    onEvent(&fsm->getFST_1_POSITION_HEIGHTMEASUREMENT_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(heightSensor1Mutex);
        Puk* puk = queuePop(&heightSensor1);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] PUK is NULLPTR", "PositionTracker.onHSPukRemoved");
        } else {
            puk->setTimers(Timer::MotorState::MOTOR_STOP);
            Logger::getInstance().log(LogLevel::DEBUG, " PUK between LBF1 and HS1 Removed PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        "PUK Type " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        "PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()), "PositionTracker.onHSPukRemoved");
        }
    });
    onEvent(&fsm->getFST_1_POSITION_SORTING_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(sorting1Mutex);
        Puk* puk = queuePop(&sorting1);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] PUK is NULLPTR", "PositionTracker.onSortingPukRemoved");
        } else {
            puk->setTimers(Timer::MotorState::MOTOR_STOP);
            Logger::getInstance().log(LogLevel::DEBUG, " PUK between HS1 and LBM1 Removed PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        "PUK Type " +  puk->pukTypeToString(puk->getPukType()) +
                                                        "PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()), "PositionTracker.onSortingPukRemoved");
        }
    });
    onEvent(&fsm->getFST_1_POSITION_EGRESS_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(egress1Mutex);
        Puk* puk = queuePop(&egress1);
        if (puk == nullptr) {
            Logger::getInstance().log(LogLevel::ERROR, "[FST1] PUK is NULLPTR", "PositionTracker.onEgressPukRemoved");
        } else {
            puk->setTimers(Timer::MotorState::MOTOR_STOP);
            Logger::getInstance().log(LogLevel::DEBUG, " PUK between LBM1 and LBE1 Removed PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        " PUK Type " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        " PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()), "PositionTracker.onEgressPukRemoved");
        }
    });
    onEvent(&fsm->getFST_2_POSITION_INGRESS_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(ingress2Mutex);
        Puk* puk = queuePop(&ingress2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] PUK is NULLPTR", "PositionTracker.onIngressPukRemoved");
        } else {
            puk->setTimers(Timer::MotorState::MOTOR_STOP);
            Logger::getInstance().log(LogLevel::DEBUG, " PUK between LBE1 and LBF2 Removed PUK-ID: " + std::to_string(puk->getPukId()) + 
                                                        "PUK Type " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        "PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()), "PositionTracker.onIngressPukRemoved");

        }
    });
    onEvent(&fsm->getFST_2_POSITION_HEIGHTMEASUREMENT_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(heightSensor2Mutex);
        Puk* puk = queuePop(&heightSensor2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] PUK is NULLPTR", "PositionTracker.onHSPukRemoved");
        } else {
            puk->setTimers(Timer::MotorState::MOTOR_STOP);
            Logger::getInstance().log(LogLevel::DEBUG, " PUK between LBF2 and HS2 Removed PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        "PUK Type " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        "PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()) + 
                                                        "PUK Average Height FST2: "+ std::to_string(fsm->getAverageHeight2()), "PositionTracker.onHSPukRemoved");
        }
    });
    onEvent(&fsm->getFST_2_POSITION_SORTING_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(sorting2Mutex);
        Puk* puk = queuePop(&sorting2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] PUK is NULLPTR", "PositionTracker.onSortingPukRemoved");
        } else {
            puk->setTimers(Timer::MotorState::MOTOR_STOP);
            Logger::getInstance().log(LogLevel::DEBUG, " PUK between HS2 and LBM2 Removed PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        "PUK Type " +  puk->pukTypeToString(puk->getPukType()) +
                                                        "PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()) + 
                                                        "PUK AverageHeight FST2: " + std::to_string(fsm->getAverageHeight2()) , "PositionTracker.onSortingPukRemoved");
        }
    });
    onEvent(&fsm->getFST_2_POSITION_EGRESS_PUK_REMOVED(), [this](){
        std::lock_guard<std::mutex> lock(egress2Mutex);
        currentSortingType2 = sortingTypePrev(currentSortingType2, FESTO2);
        Puk* puk = queuePop(&egress2);
        if (puk == nullptr){
            Logger::getInstance().log(LogLevel::ERROR, "[FST2] PUK is NULLPTR", "PositionTracker.onEgressPukRemoved");
        } else {
            puk->setTimers(Timer::MotorState::MOTOR_STOP);
            Logger::getInstance().log(LogLevel::DEBUG, " PUK between LBM2 and LBE2 Removed PUK-ID: " + std::to_string(puk->getPukId()) +
                                                        "PUK Type " +  puk->pukTypeToString(puk->getPukType()) + 
                                                        "PUK AverageHeight FST1: " + std::to_string(puk->getAverageHeight()) + 
                                                        "PUK Average Height FST2: " + std::to_string(fsm->getAverageHeight2()), "PositionTracker.onEgressPukRemoved");
        }
    });
    onEvent(&fsm->getESTOP_RECEIVED(), [this](){
        Logger::getInstance().log(LogLevel::DEBUG, "[FST2] " + 
            std::to_string(heightSensor1.size()) +
            std::to_string(sorting1.size()) +
            std::to_string(egress1.size()) +
            std::to_string(ingress2.size()) +
            std::to_string(heightSensor2.size()) +
            std::to_string(sorting2.size()) +
            std::to_string(egress2.size()), 
            "PositionTracker.onEstopReceived");
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
        currentSortingType1 = PUK_A;
        currentSortingType2 = PUK_A;
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
    onEvent(&fsm->getTIMING_LBF_1_TO_HS_1(), [this]{
        std::chrono::milliseconds timePassed;
        if (lastTimer == std::chrono::steady_clock::time_point::min()) {
            // starting timer
            lastTimer = std::chrono::steady_clock::now();
            Logger::getInstance().log(LogLevel::TRACE, "[FST1] Starting Timer", "PositionTracker.onTimingLBF1toHS1");
        } else {
            // ending timer
                timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer
            );
            Logger::getInstance().log(LogLevel::TRACE, "[FST1] Ending Timer: " + std::to_string(timePassed.count()), "PositionTracker.onTimingLBF1toHS1");
            if (motorState1.load() == Timer::MotorState::MOTOR_FAST && durations.fst1.ingress.expected.Fast.count() == 0) {
                durations.fst1.ingress.expected.Fast = timePassed - OFFSET_EXPECTED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_FAST) {
                durations.fst1.ingress.expired.Fast = timePassed + OFFSET_EXPIRED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW && durations.fst1.ingress.expected.Slow.count() == 0) {
                durations.fst1.ingress.expected.Slow = timePassed - OFFSET_EXPECTED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW) {
                durations.fst1.ingress.expired.Slow = timePassed + OFFSET_EXPIRED;
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "[FST1]", "PositionTracker.onTimingLBF1toHS1");
            }
            lastTimer = std::chrono::steady_clock::time_point::min();
        }
    });
    // getTIMING_HS_1_TO_MS1 (LBM1)
     onEvent(&fsm->getTIMING_HS_1_TO_MS_1(), [this]{
        std::chrono::milliseconds timePassed;
        if (lastTimer == std::chrono::steady_clock::time_point::min()) {
            // starting timer
            lastTimer = std::chrono::steady_clock::now();
            Logger::getInstance().log(LogLevel::TRACE, "[FST1] Starting Timer", "PositionTracker.onTimingHS1toLBM1");
        } else {
            // ending timer
            timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer
            );
            Logger::getInstance().log(LogLevel::TRACE, "[FST1] Ending Timer: " + std::to_string(timePassed.count()), "PositionTracker.onTimingHS1toLBM1");
            if (motorState1.load() == Timer::MotorState::MOTOR_FAST && durations.fst1.heightSensor.expected.Fast.count() == 0) {
                durations.fst1.heightSensor.expected.Fast = timePassed - OFFSET_EXPECTED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_FAST) {
                durations.fst1.heightSensor.expired.Fast = timePassed + OFFSET_EXPIRED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW && durations.fst1.heightSensor.expected.Slow.count() == 0) {
                durations.fst1.heightSensor.expected.Slow = timePassed - OFFSET_EXPECTED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW) {
                durations.fst1.heightSensor.expired.Slow = timePassed + OFFSET_EXPIRED;
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "[FST1]", "PositionTracker.onTimingHS1toLBM1");
            }
            lastTimer = std::chrono::steady_clock::time_point::min();
        }
    });
    // getTIMING_LBM1_TO_LBE1
    onEvent(&fsm->getTIMING_MS_1_LBE_1(), [this]{
        std::chrono::milliseconds timePassed;
        if (lastTimer == std::chrono::steady_clock::time_point::min()) {
            // starting timer
            lastTimer = std::chrono::steady_clock::now();
            Logger::getInstance().log(LogLevel::TRACE, "[FST1] Starting Timer", "PositionTracker.onTimingMS1toLBE1");
        } else {
            // ending timer
            timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer
            );
            Logger::getInstance().log(LogLevel::TRACE, "[FST1] Ending Timer: " + std::to_string(timePassed.count()), "PositionTracker.onTimingMS1toLBE1");
            if (motorState1.load() == Timer::MotorState::MOTOR_FAST && durations.fst1.sorting.expected.Fast.count() == 0) {
                durations.fst1.sorting.expected.Fast = timePassed - OFFSET_EXPECTED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_FAST) {
                durations.fst1.sorting.expired.Fast = timePassed + OFFSET_EXPIRED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW && durations.fst1.sorting.expected.Slow.count() == 0) {
                durations.fst1.sorting.expected.Slow = timePassed - OFFSET_EXPECTED;
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW) {
                durations.fst1.sorting.expired.Slow = timePassed + OFFSET_EXPIRED;
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "[FST1]", "PositionTracker.onTimingMS1toLBE1");
            }
            lastTimer = std::chrono::steady_clock::time_point::min();
        }
    });
    
    // // getTIMING_LBE1_TO_LBF2
     onEvent(&fsm->getTIMING_LBE_1_LBF_2(), [this]{
        std::chrono::milliseconds timePassed;
        if (lastTimer == std::chrono::steady_clock::time_point::min()) {
            // starting timer
            lastTimer = std::chrono::steady_clock::now();
            Logger::getInstance().log(LogLevel::TRACE, "[FST1&2] Starting Timer", "PositionTracker.onTimingLBE1toLBF2");
        } else {
            // ending timer
            timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer
            );
            Logger::getInstance().log(LogLevel::TRACE, "[FST1&2] Ending Timer: " + std::to_string(timePassed.count()), "PositionTracker.onTimingLBE1toLBF2");
            if (motorState1.load() == Timer::MotorState::MOTOR_FAST && durations.fst1.egress.expected.Fast.count() == 0) {
                durations.fst1.egress.expected.Fast = std::chrono::milliseconds(50);
            } else if (motorState1.load() == Timer::MotorState::MOTOR_FAST) {
                durations.fst1.egress.expired.Fast = std::chrono::milliseconds(2000);
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW && durations.fst1.egress.expected.Slow.count() == 0) {
                durations.fst1.egress.expected.Slow = std::chrono::milliseconds(100);
            } else if (motorState1.load() == Timer::MotorState::MOTOR_SLOW) {
                durations.fst1.egress.expired.Slow = std::chrono::milliseconds(4000);
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "[FST1&2]", "PositionTracker.onTimingLBE1toLBF2");
            }
            lastTimer = std::chrono::steady_clock::time_point::min();
        }
    });
       
    // getTIMING_LBF_2_TO_HS2
    onEvent(&fsm->getTIMING_LBF_2_TO_HS_2(), [this]{
        std::chrono::milliseconds timePassed;
        if (lastTimer == std::chrono::steady_clock::time_point::min()) {
            // starting timer
            lastTimer = std::chrono::steady_clock::now();
            Logger::getInstance().log(LogLevel::TRACE, "[FST2] Starting Timer", "PositionTracker.onTimingLBF2toHS2");
        } else {
            // ending timer
            timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer
            );
            Logger::getInstance().log(LogLevel::TRACE, "[FST2] Ending Timer: " + std::to_string(timePassed.count()), "PositionTracker.onTimingLBF2toHS2");
            if (motorState2.load() == Timer::MotorState::MOTOR_FAST && durations.fst2.ingress.expected.Fast.count() == 0) {
                durations.fst2.ingress.expected.Fast = timePassed - OFFSET_EXPECTED;
                Logger::getInstance().log(LogLevel::TRACE, "[FST2]: " + std::to_string(durations.fst2.ingress.expected.Fast.count()), "PositionTracker.onTimingLBF2toHS2"); // Hier war FOOOOOOOOOOOOOOOOO
            } else if (motorState2.load() == Timer::MotorState::MOTOR_FAST) {
                durations.fst2.ingress.expired.Fast = timePassed + OFFSET_EXPIRED;
            } else if (motorState2.load() == Timer::MotorState::MOTOR_SLOW && durations.fst2.ingress.expected.Slow.count() == 0) {
                durations.fst2.ingress.expected.Slow = timePassed - OFFSET_EXPECTED;
            } else if (motorState2.load() == Timer::MotorState::MOTOR_SLOW) {
                durations.fst2.ingress.expired.Slow = timePassed + OFFSET_EXPIRED;
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "[FST2]", "PositionTracker.onTimingLBF2toHS2");
            }
            lastTimer = std::chrono::steady_clock::time_point::min();
        }
    });
    // getTIMING_HS2_TO_LBM2
    onEvent(&fsm->getTIMING_HS_2_TO_MS_2(), [this]{
        std::chrono::milliseconds timePassed;
        if (lastTimer == std::chrono::steady_clock::time_point::min()) {
            // starting timer
            lastTimer = std::chrono::steady_clock::now();
            Logger::getInstance().log(LogLevel::TRACE, "[FST2] Starting Timer", "PositionTracker.onTimingHS2toMS2");
        } else {
            // ending timer
            timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer
            );
            Logger::getInstance().log(LogLevel::TRACE, "[FST2] Ending Timer: " + std::to_string(timePassed.count()), "PositionTracker.onTimingHS2toMS2");
            if (motorState2.load() == Timer::MotorState::MOTOR_FAST && durations.fst2.heightSensor.expected.Fast.count() == 0) {
                durations.fst2.heightSensor.expected.Fast = timePassed - OFFSET_EXPECTED;
            } else if (motorState2.load() == Timer::MotorState::MOTOR_FAST) {
                durations.fst2.heightSensor.expired.Fast = timePassed + OFFSET_EXPIRED;
            } else if (motorState2.load() == Timer::MotorState::MOTOR_SLOW && durations.fst2.heightSensor.expected.Slow.count() == 0) {
                durations.fst2.heightSensor.expected.Slow = timePassed - OFFSET_EXPECTED;
            } else if (motorState2.load() == Timer::MotorState::MOTOR_SLOW) {
                durations.fst2.heightSensor.expired.Slow = timePassed + OFFSET_EXPIRED;
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "[FST2]", "PositionTracker.onTimingHS2toMS2");
            }
            lastTimer = std::chrono::steady_clock::time_point::min();
        }
    });
    // getTIMING_LBM2_TO_LBE2
    onEvent(&fsm->getTIMING_MS_2_LBE_2(), [this]{
        std::chrono::milliseconds timePassed;
        if (lastTimer == std::chrono::steady_clock::time_point::min()) {
            // starting timer
            lastTimer = std::chrono::steady_clock::now();
            Logger::getInstance().log(LogLevel::TRACE, "[FST2] Starting Timer", "PositionTracker.onTimingMS2toLBE2");
        } else {
            // ending timer
            timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer
            );
            Logger::getInstance().log(LogLevel::TRACE, "[FST2] Ending Timer: " + std::to_string(timePassed.count()), "PositionTracker.onTimingMS2toLBE2");
           if (durations.fst2.sorting.expected.Slow.count() == 0) {
                durations.fst2.sorting.expected.Slow = timePassed - OFFSET_EXPECTED;
                // TODO need to be tested !!!
                durations.fst1.ingress.distanceValid.Slow = durations.fst1.ingress.expected.Slow  / 2;
                durations.fst2.ingress.distanceValid.Slow = durations.fst2.ingress.expected.Slow  / 2;
                durations.fst1.sorting.distanceValid.Slow = durations.fst1.sorting.expected.Slow  / 2;
                durations.fst2.sorting.distanceValid.Slow = durations.fst2.sorting.expected.Slow  / 2;
            } else if (durations.fst2.sorting.expected.Fast.count() == 0) {
                durations.fst2.sorting.expected.Fast = timePassed - OFFSET_EXPECTED;
                // TODO need to be tested !!!
                durations.fst1.ingress.distanceValid.Fast = durations.fst1.ingress.expected.Fast  / 2;
                durations.fst2.ingress.distanceValid.Fast = durations.fst2.ingress.expected.Fast  / 2;
                durations.fst1.sorting.distanceValid.Fast = durations.fst1.sorting.expected.Fast  / 2;
                durations.fst2.sorting.distanceValid.Fast = durations.fst2.sorting.expected.Fast  / 2;
            } else if (durations.fst2.sorting.expired.Slow.count() == 0) {
                durations.fst2.sorting.expired.Slow = timePassed + OFFSET_EXPIRED;
            } else if (durations.fst2.sorting.expired.Fast.count() == 0) {
                durations.fst2.sorting.expired.Fast = timePassed + OFFSET_EXPIRED;
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "[FST2]", "PositionTracker.onTimingMS2toLBE2");
            }

            Logger::getInstance().log(LogLevel::WARNING,
                "\ndurations.fst1.ingress.distanceValid.Fast:" + std::to_string(durations.fst1.ingress.distanceValid.Fast.count()) + "ms" +
                "\ndurations.fst1.ingress.distanceValid.Slow:" + std::to_string(durations.fst1.ingress.distanceValid.Slow.count()) + "ms" +
                "\ndurations.fst1.ingress.expected.Fast:" + std::to_string(durations.fst1.ingress.expected.Fast.count()) + "ms" +
                "\ndurations.fst1.ingress.expected.Slow:" + std::to_string(durations.fst1.ingress.expected.Slow.count()) + "ms" +
                "\ndurations.fst1.ingress.expired.Fast:" + std::to_string(durations.fst1.ingress.expired.Fast.count()) + "ms" +
                "\ndurations.fst1.ingress.expired.Slow:" + std::to_string(durations.fst1.ingress.expired.Slow.count()) + "ms" +
                "\ndurations.fst1.heightSensor.expected.Fast:" + std::to_string(durations.fst1.heightSensor.expected.Fast.count()) + "ms" +
                "\ndurations.fst1.heightSensor.expected.Slow:" + std::to_string(durations.fst1.heightSensor.expected.Slow.count()) + "ms" +
                "\ndurations.fst1.heightSensor.expired.Fast:" + std::to_string(durations.fst1.heightSensor.expired.Fast.count()) + "ms" +
                "\ndurations.fst1.heightSensor.expired.Slow:" + std::to_string(durations.fst1.heightSensor.expired.Slow.count()) + "ms" +
                "\ndurations.fst1.sorting.distanceValid.Fast:" + std::to_string(durations.fst1.sorting.distanceValid.Fast.count()) + "ms" +
                "\ndurations.fst1.sorting.distanceValid.Slow:" + std::to_string(durations.fst1.sorting.distanceValid.Slow.count()) + "ms" +
                "\ndurations.fst1.sorting.expected.Fast:" + std::to_string(durations.fst1.sorting.expected.Fast.count()) + "ms" +
                "\ndurations.fst1.sorting.expected.Slow:" + std::to_string(durations.fst1.sorting.expected.Slow.count()) + "ms" +
                "\ndurations.fst1.sorting.expired.Fast:" + std::to_string(durations.fst1.sorting.expired.Fast.count()) + "ms" +
                "\ndurations.fst1.sorting.expired.Slow:" + std::to_string(durations.fst1.sorting.expired.Slow.count()) + "ms" +
                "\ndurations.fst1.egress.expected.Fast:" + std::to_string(durations.fst1.egress.expected.Fast.count()) + "ms" +
                "\ndurations.fst1.egress.expected.Slow:" + std::to_string(durations.fst1.egress.expected.Slow.count()) + "ms" +
                "\ndurations.fst1.egress.expired.Fast:" + std::to_string(durations.fst1.egress.expired.Fast.count()) + "ms" +
                "\ndurations.fst1.egress.expired.Slow:" + std::to_string(durations.fst1.egress.expired.Slow.count()) + "ms" + 
                "\ndurations.fst2.ingress.distanceValid.Fast:" + std::to_string(durations.fst2.ingress.distanceValid.Fast.count()) + "ms" +
                "\ndurations.fst2.ingress.distanceValid.Slow:" + std::to_string(durations.fst2.ingress.distanceValid.Slow.count()) + "ms" +
                "\ndurations.fst2.ingress.expected.Fast:" + std::to_string(durations.fst2.ingress.expected.Fast.count()) + "ms" +
                "\ndurations.fst2.ingress.expected.Slow:" + std::to_string(durations.fst2.ingress.expected.Slow.count()) + "ms" +
                "\ndurations.fst2.ingress.expired.Fast:" + std::to_string(durations.fst2.ingress.expired.Fast.count()) + "ms" +
                "\ndurations.fst2.ingress.expired.Slow:" + std::to_string(durations.fst2.ingress.expired.Slow.count()) + "ms" +
                "\ndurations.fst2.heightSensor.expected.Fast:" + std::to_string(durations.fst2.heightSensor.expected.Fast.count()) + "ms" +
                "\ndurations.fst2.heightSensor.expected.Slow:" + std::to_string(durations.fst2.heightSensor.expected.Slow.count()) + "ms" +
                "\ndurations.fst2.heightSensor.expired.Fast:" + std::to_string(durations.fst2.heightSensor.expired.Fast.count()) + "ms" +
                "\ndurations.fst2.heightSensor.expired.Slow:" + std::to_string(durations.fst2.heightSensor.expired.Slow.count()) + "ms" +
                "\ndurations.fst2.sorting.distanceValid.Fast:" + std::to_string(durations.fst2.sorting.distanceValid.Fast.count()) + "ms" +
                "\ndurations.fst2.sorting.distanceValid.Slow:" + std::to_string(durations.fst2.sorting.distanceValid.Slow.count()) + "ms" +
                "\ndurations.fst2.sorting.expected.Fast:" + std::to_string(durations.fst2.sorting.expected.Fast.count()) + "ms" +
                "\ndurations.fst2.sorting.expected.Slow:" + std::to_string(durations.fst2.sorting.expected.Slow.count()) + "ms" +
                "\ndurations.fst2.sorting.expired.Fast:" + std::to_string(durations.fst2.sorting.expired.Fast.count()) + "ms" +
                "\ndurations.fst2.sorting.expired.Slow:" + std::to_string(durations.fst2.sorting.expired.Slow.count()) + "ms", "HIER");

            lastTimer = std::chrono::steady_clock::time_point::min();
        }

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
        Logger::getInstance().log(LogLevel::TRACE, "[FST2] Motor Slow", "PositionTracker.onMotorSlow");
    }
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
                // {
                //     // Puk* puk = egress1.front();
                //     if (diverter1TimeOut == nullptr) {
                //         Logger::getInstance().log(LogLevel::ERROR, "PROBLEMSS", "PositionTracker.listen");
                //     } else {
                //         diverter1TimeOut->kill();
                //     }
                // }
                fsm->raiseFST_1_POSITION_DIVERTER_DISTANCE_VALID();
                break;
            case Timer::PulseCode::PULSE_SORTING_1_TIMEOUT_DIVERTER:
                pulseName = "PULSE_SORTING_1_TIMEOUT_DIVERTER";
                fsm->raiseDIVERTER_TIMEOUT();
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
                // {
                //     // Puk* puk = egress2.front();
                //     if (diverter2TimeOut == nullptr) {
                //         Logger::getInstance().log(LogLevel::ERROR, "PROBLEMSS", "PositionTracker.listen");
                //     } else {
                //         diverter2TimeOut->kill();
                //     }
                // }
                fsm->raiseFST_2_POSITION_DIVERTER_DISTANCE_VALID();
                break;
            case Timer::PulseCode::PULSE_SORTING_2_TIMEOUT_DIVERTER:
                pulseName = "PULSE_SORTING_2_TIMEOUT_DIVERTER";
                fsm->raiseDIVERTER_TIMEOUT();
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

std::chrono::milliseconds PositionTracker::getDuration(int festoId, SegmentType segmentType, DurationType durationType, Timer::MotorState motorState) {
    std::chrono::milliseconds duration = std::chrono::milliseconds(0);
    if (motorState == Timer::MotorState::MOTOR_STOP) return duration;
    static const std::map<std::tuple<int, SegmentType, DurationType, Timer::MotorState>, std::chrono::milliseconds> combination_map = {
        { {FESTO1, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_FAST}, durations.fst1.ingress.distanceValid.Fast },
        { {FESTO1, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_SLOW}, durations.fst1.ingress.distanceValid.Slow },
        { {FESTO1, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst1.ingress.expected.Fast },
        { {FESTO1, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.ingress.expected.Slow },
        { {FESTO1, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst1.ingress.expired.Fast },
        { {FESTO1, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.ingress.expired.Slow },

        { {FESTO1, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst1.heightSensor.expected.Fast },
        { {FESTO1, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.heightSensor.expected.Slow },
        { {FESTO1, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst1.heightSensor.expired.Fast },
        { {FESTO1, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.heightSensor.expired.Slow },

        { {FESTO1, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_FAST}, durations.fst1.sorting.distanceValid.Fast },
        { {FESTO1, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_SLOW}, durations.fst1.sorting.distanceValid.Slow },
        { {FESTO1, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst1.sorting.expected.Fast },
        { {FESTO1, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.sorting.expected.Slow },
        { {FESTO1, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst1.sorting.expired.Fast },
        { {FESTO1, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.sorting.expired.Slow },

        { {FESTO1, SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst1.egress.expected.Fast },
        { {FESTO1, SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.egress.expected.Slow },
        { {FESTO1, SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst1.egress.expired.Fast },
        { {FESTO1, SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst1.egress.expired.Slow },

        { {FESTO2, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_FAST}, durations.fst2.ingress.distanceValid.Fast },
        { {FESTO2, SEGMENT_INGRESS, DURATION_VALID, Timer::MotorState::MOTOR_SLOW}, durations.fst2.ingress.distanceValid.Slow },
        { {FESTO2, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst2.ingress.expected.Fast },
        { {FESTO2, SEGMENT_INGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.ingress.expected.Slow },
        { {FESTO2, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst2.ingress.expired.Fast },
        { {FESTO2, SEGMENT_INGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.ingress.expired.Slow },

        { {FESTO2, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst2.heightSensor.expected.Fast },
        { {FESTO2, SEGMENT_HS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.heightSensor.expected.Slow },
        { {FESTO2, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst2.heightSensor.expired.Fast },
        { {FESTO2, SEGMENT_HS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.heightSensor.expired.Slow },

        { {FESTO2, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_FAST}, durations.fst2.sorting.distanceValid.Fast },
        { {FESTO2, SEGMENT_SORTING, DURATION_VALID, Timer::MotorState::MOTOR_SLOW}, durations.fst2.sorting.distanceValid.Slow },
        { {FESTO2, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst2.sorting.expected.Fast },
        { {FESTO2, SEGMENT_SORTING, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.sorting.expected.Slow },
        { {FESTO2, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst2.sorting.expired.Fast },
        { {FESTO2, SEGMENT_SORTING, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.sorting.expired.Slow },

        { {FESTO2, SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_FAST}, durations.fst2.egress.expected.Fast },
        { {FESTO2, SEGMENT_EGRESS, DURATION_EXPECTED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.egress.expected.Slow },
        { {FESTO2, SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_FAST}, durations.fst2.egress.expired.Fast },
        { {FESTO2, SEGMENT_EGRESS, DURATION_EXPIRED, Timer::MotorState::MOTOR_SLOW}, durations.fst2.egress.expired.Slow },
    };

    // Look up the combination and print the result
    auto it = combination_map.find(std::make_tuple(festoId, segmentType, durationType, motorState));
    if (it != combination_map.end()) {
        duration = it->second;
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to map SegmentType/DurationType/MotorState to duration", "PositionTracker.getDuration");
    }
    // Logger::getInstance().log(LogLevel::DEBUG, "Duration:" + std::to_string(duration.count()), "PositionTracker.getDuration");
    return duration;
}

uint32_t PositionTracker::nextPukId() {
    uint32_t pukId = lastPukId.load();
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

PositionTracker::SortingType PositionTracker::sortingTypeNext(SortingType st, int festoId) {
    if (st == PUK_A){
        Logger::getInstance().log(LogLevel::DEBUG, "[FST" + std::to_string(festoId+1) + "]SORTING EXPECTED PUK_A -> PUK_B", "PositionTracker.sortingTypeNext");
        return SortingType::PUK_B;
    } else if (st == PUK_B){
        Logger::getInstance().log(LogLevel::DEBUG, "[FST" + std::to_string(festoId+1) + "]SORTING EXPECTED PUK_B -> PUK_C", "PositionTracker.sortingTypeNext");
        return SortingType::PUK_C;
    } else if (st == PUK_C){
        Logger::getInstance().log(LogLevel::DEBUG, "[FST" + std::to_string(festoId+1) + "]SORTING EXPECTED PUK_C -> PUK_A", "PositionTracker.sortingTypeNext");
        return SortingType::PUK_A;
    } else {
        Logger::getInstance().log(LogLevel::DEBUG, "UNknown type", "PositionTracker.sortingTypeNext");
        return SortingType::PUK_A;
    }
}

PositionTracker::SortingType PositionTracker::sortingTypePrev(SortingType st, int festoId) {
    if (st == PUK_A){
        Logger::getInstance().log(LogLevel::DEBUG, "[FST" + std::to_string(festoId+1) + "]SORTING EXPECTED PUK_A -> PUK_C", "PositionTracker.sortingTypeNext");
        return SortingType::PUK_C;
    } else if (st == PUK_B){
        Logger::getInstance().log(LogLevel::DEBUG, "[FST" + std::to_string(festoId+1) + "]SORTING EXPECTED PUK_B -> PUK_A", "PositionTracker.sortingTypeNext");
        return SortingType::PUK_A;
    } else if (st == PUK_C){
        Logger::getInstance().log(LogLevel::DEBUG, "[FST" + std::to_string(festoId+1) + "]SORTING EXPECTED PUK_C -> PUK_B", "PositionTracker.sortingTypeNext");
        return SortingType::PUK_B;
    } else {
        Logger::getInstance().log(LogLevel::DEBUG, "UNknown type", "PositionTracker.sortingTypePrev");
        return SortingType::PUK_A;
    }
}