/*
 * Dispatcher.h
 *
 *  Created on: 05.12.2024
 *      Author: Marc
 */

#ifndef FSM_H
#define FSM_H

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"

#define FSM_NUM_OF_PULSES 16

class FSM : public PulseMsgHandler {
  public:
    FSM(const std::string name);
    ~FSM();

    void handleMsg() override;
    void sendMsg(int8_t msgCode, int32_t msgValue) override;
    int32_t getChannel() override;

    int8_t* getPulses();
    int8_t getNumOfPulses();

  private:
    int32_t channelID;
    name_attach_t *fsmChannel;
    static int8_t numOfPulses;
    static int8_t pulses[FSM_NUM_OF_PULSES];
};

#endif // FSM_H
