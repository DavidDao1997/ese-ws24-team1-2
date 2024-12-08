/*
 * ActuatorController.cpp
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#include "headers/ActuatorController.h"

int8_t ActuatorController::numOfPulses = ACTUATOR_CONTROLLER_NUM_OF_PULSES;
int8_t ActuatorController::pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES] = {
    PULSE_MOTOR1_STOP,
    PULSE_MOTOR1_SLOW,
    PULSE_MOTOR1_FAST,
    PULSE_MOTOR2_STOP,
    PULSE_MOTOR2_SLOW,
    PULSE_MOTOR2_FAST,
    PULSE_LR1_ON,
    PULSE_LR1_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LR1_OFF,
    PULSE_LY1_ON,
    PULSE_LY1_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LY1_OFF,
    PULSE_LG1_ON,
    PULSE_LG1_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LG1_OFF,
    PULSE_LR2_ON,
    PULSE_LR2_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LR2_OFF,
    PULSE_LY2_ON,
    PULSE_LY2_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LY2_OFF,
    PULSE_LG2_ON,
    PULSE_LG2_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LG2_OFF,
    PULSE_Q11_ON,
    PULSE_Q11_OFF,
    PULSE_Q12_ON,
    PULSE_Q12_OFF,
    PULSE_SM1_OPEN, 
    PULSE_SM1_CLOSE,
    PULSE_SM2_OPEN,
    PULSE_SM2_CLOSE
};

ActuatorController::ActuatorController(const std::string name, Actuators_Wrapper *actuatorsWrapper) {
    actuatorControllerChannel = createNamedChannel(name);
    channelID = actuatorControllerChannel->chid;
    actuators = actuatorsWrapper;
    running = false;
};

ActuatorController::~ActuatorController() {
    // thread löschen
    running = false;
    // attach löschen
    destroyNamedChannel(channelID, actuatorControllerChannel);
};

int8_t* ActuatorController::getPulses() { return pulses; };
int8_t ActuatorController::getNumOfPulses() { return numOfPulses; };

void ActuatorController::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received
            switch (msg.code) {
            case PULSE_MOTOR1_STOP:
                std::cout << "ACTUATORCONTROLLER: Motor will be stopped" << std::endl;
                actuators->motorStop();
                break;
            // case PULSE_MOTOR1_START:
            //     std::cout << "ACTUATORCONTROLLER: Motor will start running right" << std::endl;
            //     actuators->runRight();
            //     break;
            case PULSE_MOTOR1_SLOW:
                std::cout << "ACTUATORCONTROLLER: Motor will be slow running" << std::endl;
                actuators->runSlow();
                actuators->runRight();  // TODO test is toggle button is not needed
                break;
            case PULSE_MOTOR1_FAST:
                std::cout << "ACTUATORCONTROLLER: Motor will be fast running" << std::endl;
                actuators->runFast();
                actuators->runRight();
                break;
            }
        }
    }
}

int32_t ActuatorController::getChannel() { return channelID; }

void ActuatorController::sendMsg() {} // keep empty, not needed
