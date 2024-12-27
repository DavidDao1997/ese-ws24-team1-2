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

#include "../../headers/FSMEgress.h"
#include "../../headers/FSMHeightMeasurement.h"
#include "../../headers/FSMIngress.h"
#include "../../headers/FSMLampGreen.h"
#include "../../headers/FSMLampRed.h"
#include "../../headers/FSMLampYellow.h"
#include "../../headers/FSMMotor.h"
#include "../../headers/FSMSystem.h"
#include "../../headers/FSMSorting.h"

#include "../../../Logging/headers/Logger.h"

#define FSM_CONTROLLER_NUM_OF_PULSES 20

class FSMController : public PulseMsgHandler {
  public:
    FSMController(const std::string dispatcherChannelName);
    virtual ~FSMController();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    int8_t *getPulses();
    int8_t getNumOfPulses();
    bool stop();

  private:
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
    FSMSorting *fsmSorting;

    int32_t channelID;
    int32_t dispatcherConnectionID;

    bool running;
    bool subThreadsRunning;
    static int8_t numOfPulses;
    static int8_t pulses[FSM_CONTROLLER_NUM_OF_PULSES];
};

#endif /* LOGIK_FSM_FSMCONTROLLER_H_ */
