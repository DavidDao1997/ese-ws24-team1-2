/*
 * FSMController.h
 *
 *  Created on: 07.12.2024
 *      Author: Marc
 */

#ifndef LOGIK_FSM_FSMCONTROLLER_H_
#define LOGIK_FSM_FSMCONTROLLER_H_

#include <functional> // For std::bind

#include "../../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../../FSM_QualityGate/src-gen/FSM_QualityGate.h"
#include "../../../FSM_QualityGate/src/sc_rxcpp.h"

#include "../../headers/FSMEgress.h"
#include "../../headers/FSMHeightMeasurement.h"
#include "../../headers/FSMIngress.h"
#include "../../headers/FSMLampGreen.h"
#include "../../headers/FSMLampRed.h"
#include "../../headers/FSMLampYellow.h"
#include "../../headers/FSMMotor.h"
#include "../../headers/FSMSystem.h"

#define FSM_CONTROLLER_NUM_OF_PULSES 15

class FSMController : public PulseMsgHandler {
  public:
    FSMController(const std::string dispatcherChannelName);
    virtual ~FSMController();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    int8_t *getPulses();
    int8_t getNumOfPulses();

  private:
    FSM_QualityGate *fsm;
    // FSM System
    FSMSystem *fsmSystem;
    FSMMotor *fsmMotor;
    // FSM LED
    FSMLampGreen *fsmLG1;
    FSMLampRed *fsmLR1;
    FSMLampYellow *fsmLY1;
    FSMIngress *fsmIngress;
    FSMHeightMeasurement *fsmHeightMeasurement;
    FSMEgress *fsmEgress;

    int32_t channelID;
    int32_t dispatcherConnectionID;

    bool running;
    bool subThreadsRunning;
    static int8_t numOfPulses;
    static int8_t pulses[FSM_CONTROLLER_NUM_OF_PULSES];

    static void monitorObservable(
        sc::rx::Observable<void> observable,
        std::string pulseName,
        PulseCode pulseCode,
        int32_t pulseValue,
        bool subThreadsRunning,
        int32_t dispatcherConnectionID
    );
};

#endif /* LOGIK_FSM_FSMCONTROLLER_H_ */
