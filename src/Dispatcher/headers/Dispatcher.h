/*
 * Dispatcher.h
 *
 *  Created on: 21.11.2024
 *      Author: Marc
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <cstdio> // For sprintf
#include <iostream>
#include <map>
#include <stdint.h>
#include <string>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <thread>
#include <vector>

#include "PulseMsgConfig.h"
#include "PulseMsgHandler.h"

class Dispatcher : public PulseMsgHandler {
  public:
    Dispatcher(const std::string name);
    ~Dispatcher();

    void handleMsg() override;
    void sendMsg(int8_t msgCode, int32_t msgValue) override;
    int32_t getChannel() override;

    void addSubscriber(int32_t chid, int8_t pulses[], int8_t numOfPulses);

  private:
    int32_t channelID;
    name_attach_t *dispatcherChannel;
    std::vector<int32_t> connections;
    std::map<uint8_t, std::vector<int32_t>> connectionsByPulse;
    bool running;
};

#endif // DISPATCHER_H
