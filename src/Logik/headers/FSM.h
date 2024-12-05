/*
 * Dispatcher.h
 *
 *  Created on: 05.12.2024
 *      Author: Marc
 */

#ifndef FSM_H
#define FSM_H

#include "../../Dispatcher/header/PulseMsgConfig.h"
#include "../../Dispatcher/header/PulseMsgHandler.h"

class FSM : public PulseMsgHandler {
  public:
    FSM(const std::string name);
    ~FSM();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

  private:
    int32_t channelID;
    name_attach_t *fsmChannel;
};

#endif // FSM_H
