#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSM.h"

int8_t FSM::numOfPulses = FSM_NUM_OF_PULSES;
int8_t FSM::pulses[FSM_NUM_OF_PULSES] = {
    PULSE_ESTOP_HIGH,
    PULSE_ESTOP_LOW,
    PULSE_LBF_INTERRUPTED,
    PULSE_LBF_OPEN,
    PULSE_LBE_INTERRUPTED,
    PULSE_LBE_OPEN,
    PULSE_LBR_INTERRUPTED,
    PULSE_LBR_OPEN,
    PULSE_LBM_INTERRUPTED,
    PULSE_LBM_OPEN
};

FSM::FSM(const std::string name) {
    fsmChannel = createNamedChannel(name);
    channelID = fsmChannel->chid;
}
FSM::~FSM() {}

/*
    this routine tests pulse message communication via dispatcher.
    It sends alternating motorStart/moterStop pulses to the actuatorController every 3 seconds.
*/

int8_t* FSM::getPulses() { return pulses; };
int8_t FSM::getNumOfPulses() { return numOfPulses; };

void FSM::handleMsg() {
    // Fetch dispatcherConnectionID
    std::string dispatcherChannelName = "dispatcher"; // FIXME hardcoded
    int dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    if (dispatcherConnectionID == -1) {
        perror("FSM: name_open failed");
        return;
    }

    _pulse msg;
    while (1) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);
        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        switch (msg.code)
        {
        case PULSE_LBF_INTERRUPTED:
            if(0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_FAST, 0)) {
                perror("FSM: MsgSendPulse failed");
            }
            break;
        case PULSE_LBE_INTERRUPTED:
            if(0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_STOP, 0)) {
                perror("FSM: MsgSendPulse failed");
            }
            break;
        default:
            perror("should not happen");
            break;
        }

        // logging
        // char buffer[100];
        // sprintf(
        //     buffer,
        //     "FSM: Publishing pulse %s(code: %d)\n",
        //     motorRunning ? "PULSE_MOTOR1_STOP" : "PULSE_MOTOR1_START",
        //     motorRunning ? PULSE_MOTOR1_STOP : PULSE_MOTOR1_START
        // );
        // std::cout << buffer << std::flush;

        // // Send pulse
        // int err = MsgSendPulse(dispatcherConnectionID, -1, motorRunning ? PULSE_MOTOR1_STOP : PULSE_MOTOR1_START, 0);
        // if (err == -1) {
        //     perror("FSM: MsgSendPulse failed");
        // } else {
        //     motorRunning = !motorRunning;
        // }

        // Sleep
        // std::cout << "FSM sleeping for 3 second " << std::endl;
        // WAIT(1000000);
    }
}

void FSM::sendMsg() {}

int32_t FSM::getChannel() { return channelID; }
