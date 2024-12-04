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

#include "../Dispatcher/PulseMsgHandler.h"

typedef struct allSubcribers {
    int numOfSubs;
    void * subs[];
} allSubcribers_t;

class Dispatcher : public PulseMsgHandler{
public:

    Dispatcher(const char* name); // liste mit allen erreichbaren teilnehmer müssen hier hin. eigener typ?? zb ein array vom typ allSubscribers?
    ~Dispatcher();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    void connectToAllChannels();
    

private:
    int32_t channelID;
    name_attach_t* dispatcherChannel;
    allSubcribers_t allSubs;

};

#endif // DISPATCHER_H
