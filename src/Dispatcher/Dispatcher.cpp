/*
 * Dispatcher.cpp
 *
 *  Created on: 21.11.2024
 *      Author: Marc
 */


#include "header/Dispatcher.h"
#include "../HAL/halheader/SensorISR.h"


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

void Dispatcher::addSubscriber(int32_t coid, int8_t pulses[], int8_t numOfPulses) {
    for (uint8_t i = 0; i < numOfPulses; ++i) {
        uint8_t pulse = pulses[i];
        channelsByPulse[pulse].push_back(coid);
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
            switch (msg.code){
                case PULSE_STOP_THREAD: 
                    running = false;
                    break;
                //case PULSE_DISPATCHER_SUBSCRIBE:
                    //std::cout << "subscribing" << std::endl;
            	    // coids add msg.coid;
                    // events = msg.events;
                    /*
                    map[key]value
                    {
                        "10234": PULSE_DISPATCHER_SUBSCRIBE, PULSE_DISPATCHER_SUBSCRIBE,PULSE_DISPATCHER_SUBSCRIBE
                    }
                    */
                    //break;
                default:
                    // Schaue in map wer sich für msg.code interessiert und schicke an diese
                    auto coids = channelsByPulse.find(msg.code);
                    if (coids != channelsByPulse.end()) {
                        for (const auto& coid : coids->second){
                            MsgSendPulse(coid, -1, msg.code, 0);
                        }
                    }            	    
            }
        }
    }
}

void Dispatcher::sendMsg(){}


int32_t Dispatcher::getChannel(){
    return channelID;
}
