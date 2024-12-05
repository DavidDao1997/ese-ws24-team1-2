/*
 * ActuatorController.cpp
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#include "headers/ActuatorController.h"

int8_t ActuatorController::numOfPulses = ACTUATOR_CONTROLLER_NUM_OF_PULSES;
int8_t ActuatorController::pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES] = {
    PULSE_MOTOR_STOP,
    PULSE_MOTOR_START,
    PULSE_MOTOR_SLOW,
    PULSE_MOTOR_FAST,
};

ActuatorController::ActuatorController(const std::string name, Actuators_Wrapper *actuatorsWrapper) {
    actConChannel = createNamedChannel(name);
    channelID = actConChannel->chid;
    actuators = actuatorsWrapper;
};

ActuatorController::~ActuatorController() {
    // thread löschen
    int connectionID = connectToChannel(channelID);
    if (connectionID >= 0) {
        MsgSendPulse(connectionID, -1, PULSE_STOP_THREAD, 0);
        ConnectDetach(connectionID);
    }
    // attach löschen
    destroyNamedChannel(channelID, actConChannel);
};

void ActuatorController::handleMsg() {
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
            switch (msg.code) {
            case PULSE_STOP_THREAD:
                running = false;
                break;
            case PULSE_MOTOR_STOP:
                std::cout << "ACTUATORCONTROLLER: Motor will be stopped" << std::endl;
                actuators->motorStop();
                break;
            case PULSE_MOTOR_START:
                std::cout << "ACTUATORCONTROLLER: Motor will start running right" << std::endl;
                actuators->runRight();
                break;
            case PULSE_MOTOR_SLOW:
                std::cout << "ACTUATORCONTROLLER: Motor will be slow running" << std::endl;
                actuators->runSlow();
                break;
            case PULSE_MOTOR_FAST:
                std::cout << "ACTUATORCONTROLLER: Motor will be fast running" << std::endl;
                actuators->runFast();
                break;
            }
        }
    }
}

int32_t ActuatorController::getChannel() { return channelID; }

void ActuatorController::sendMsg(int8_t msgCode, int32_t msgValue) {} // keep empty, not needed
