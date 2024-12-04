/*
 * Dispatcher.h
 *
 *  Created on: 21.11.2024
 *      Author: Marc
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <map>
#include <vector>
#include <string>


#include "PulseMsgHandler.h"
#include "PulseMsgConfig.h"



class Dispatcher : public PulseMsgHandler{
public:

    Dispatcher(const std::string name); // liste mit allen erreichbaren teilnehmer müssen hier hin. eigener typ?? zb ein array vom typ allSubscribers?
    ~Dispatcher();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    void addSubscriber(int32_t coid, uint8_t pulses[], int8_t numOfPulses);
    

private:
    int32_t channelID;
    name_attach_t* dispatcherChannel;
    std::map<uint8_t, std::vector<int32_t>> channelsByPulse;

};

#endif // DISPATCHER_H
