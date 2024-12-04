/*
 * Dispatcher.cpp
 *
 *  Created on: 21.11.2024
 *      Author: Marc
 */


#include "festoheader/Dispatcher.h"
#include "HAL/halheader/HALConfig.h"
#include "HAL/halheader/SensorISR.h"


#include <stdint.h>
#include "sys/mman.h"
#include "sys/neutrino.h"
#include "hw/inout.h"
#include <sys/procmgr.h>
#include <thread>


Dispatcher::Dispatcher(const char* name) {
    dispatcherChannel = createNamedChannel(name);
    channelID = dispatcherChannel->chid;
}

Dispatcher::~Dispatcher() {
    // Send stop pulse to terminate any dispatch thread
    int connectionID = ConnectAttach(0, 0, channelID, _NTO_SIDE_CHANNEL, 0);
    if (connectionID >= 0) {
        MsgSendPulse(connectionID, -1, PULSE_STOP_THREAD, 0);
        ConnectDetach(connectionID);
    } else {
        perror("Failed to connect for stop pulse!");
    }

    // Destroy the channel
    destroyNamedChannel(channelID, dispatcherChannel);
}

void Dispatcher::connectToAllChannels(){
    //int DecID = connectToChannel(decoder->getChannel()); // TODO Decoder usw muss schon existieren !!!!!!!!
    // usw
    int connectionIDs[allSubs.numOfSubs];
    for (int i= 0; i < allSubs.numOfSubs; i++){
        //connectionIDs[i] = connectToChannel(allSubs.subs[i]->getChannel()); // funktioniert so leider nicht mit void*
    }
}


void Dispatcher::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    bool running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);  // TODO exit??? was passiert wenn der dispatcher stirbt. fehlerbhandlung?
        }

        if (recvid == 0) { // Pulse received
            if (msg.code == PULSE_STOP_THREAD) {
                running = false;
            }

            else {
                // iteriere durch alle angebundenen channels und hau die nachricht raus.
                // Der jeweilige empfänger entscheided ob nachricht sinnvoll
            }
        }
    }
}

void Dispatcher::sendMsg(){


    
}

int32_t Dispatcher::getChannel(){
    return channelID;
}