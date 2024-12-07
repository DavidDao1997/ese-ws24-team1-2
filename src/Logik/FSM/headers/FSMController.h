/*
 * FSMController.h
 *
 *  Created on: 07.12.2024
 *      Author: Marc
 */

#ifndef LOGIK_FSM_FSMCONTROLLER_H_
#define LOGIK_FSM_FSMCONTROLLER_H_


#include "../../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../../FSM_QualityGate/src-gen/FSM_QualityGate.h"

#define FSM_CONTROLLER_NUM_OF_PULSES 16

class FSMController : public PulseMsgHandler {
public:

FSMController(int32_t dispatcherChannel);
virtual ~FSMController();

void handleMsg() override;
void sendMsg(int8_t msgCode, int32_t msgValue) override;
int32_t getChannel() override;


private:



FSM_QualityGate* fsm;
int32_t channelID;
int32_t dispatcherChannelID;

bool running;
static int8_t numOfPulses;
static int8_t pulses[FSM_CONTROLLER_NUM_OF_PULSES];

};



#endif /* LOGIK_FSM_FSMCONTROLLER_H_ */
