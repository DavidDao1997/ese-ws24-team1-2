/*
 * ActuatorController.cpp
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */


#include "festoheader/ActuatorController.h"


ActuatorController::ActuatorController(const char* name, Actuators_Wrapper* actuatorsWrapper){
    actConChannel = createNamedChannel(name);
    channelID = actConChannel->chid;
    actuators = actuatorsWrapper;
}


ActuatorController::~ActuatorController(){
    // thread löschen
    int connectionID = connectToChannel(channelID);
    if (connectionID >= 0) {
        MsgSendPulse(connectionID, -1, PULSE_STOP_THREAD, 0);
        ConnectDetach(connectionID);
    }
    // attach löschen
    destroyNamedChannel(channelID, actConChannel);
}


void ActuatorController::handleMsg(){
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    bool running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        
        if (recvid == 0) { // Pulse received
            switch (msg.code){
                case PULSE_STOP_THREAD: 
                    running = false;
                    break;
                case PULSE_MOTOR_STOP:
                    std::cout << "Motor will be stopped" << std::endl;
            	    actuators->motorStop();
                    break;
                case PULSE_MOTOR_START:
                    std::cout << "Motor will be start running right" << std::endl;
            	    actuators->runRight();
                    break;
                case PULSE_MOTOR_SLOW:
                    std::cout << "Motor will be slow running" << std::endl;
            	    actuators->runSlow();
                    break;
                case PULSE_MOTOR_FAST:
                    std::cout << "Motor will be fast running" << std::endl;
            	    actuators->runFast();
                    break;
            }
        }
    }

}

int32_t ActuatorController::getChannel(){
    return channelID;
}



void ActuatorController::sendMsg(){} // keep empty, not needed

