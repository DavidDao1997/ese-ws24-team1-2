#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSM.h"

FSM::FSM(const std::string name) {}
FSM::~FSM() {}

/*
    this routine tests pulse message communication via dispatcher.
    It sends alternating motorStart/moterStop pulses to the actuatorController every 3 seconds.
*/
void FSM::handleMsg() {
    // Fetch dispatcherConnectionID
    std::string dispatcherChannelName = "dispatcher"; // FIXME hardcoded
    int dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    if (dispatcherConnectionID == -1) {
        perror("FSM: name_open failed");
        return;
    }

    bool motorRunning = false;
    while (1) {
        // logging
        char buffer[100];
        sprintf(
            buffer,
            "FSM: Publishing pulse %s(code: %d)\n",
            motorRunning ? "PULSE_MOTOR1_STOP" : "PULSE_MOTOR1_START",
            motorRunning ? PULSE_MOTOR1_STOP : PULSE_MOTOR1_START
        );
        std::cout << buffer << std::flush;

        // Send pulse
        int err = MsgSendPulse(dispatcherConnectionID, -1, motorRunning ? PULSE_MOTOR1_STOP : PULSE_MOTOR1_START, 0);
        if (err == -1) {
            perror("FSM: MsgSendPulse failed");
        } else {
            motorRunning = !motorRunning;
        }

        // Sleep
        // std::cout << "FSM sleeping for 3 second " << std::endl;
        WAIT(3000);
    }
}

void FSM::sendMsg(int8_t msgCode, int32_t msgValue) {}

int32_t FSM::getChannel() { return channelID; }
