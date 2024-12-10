#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"

#include "headers/FSMLampRed.h"

FSMLampRed::FSMLampRed(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    // setState(LampRedState::Off);
    currentState = LampRedState::Off;
}
FSMLampRed::~FSMLampRed() {}

void FSMLampRed::onLR1On(std::function<void(int32_t conId)> callBackFunction) { callbackLR1On = callBackFunction; }

void FSMLampRed::onLR1Off(std::function<void(int32_t conId)> callBackFunction) { callbackLR1Off = callBackFunction; }

void FSMLampRed::raiseEStopReceived() {
    if (currentState == LampRedState::Off) {
     std::cout << "FSMLAMPRED: ESTOP Received" << std::endl;
     setState(LampRedState::Constant);
    }
}
void FSMLampRed::raiseEStopCleared() {
    if (currentState == LampRedState::Constant) {
        std::cout << "FSMLAMPRED: ESTOP Cleared" << std::endl;
        setState(LampRedState::Off);
    }
}

void FSMLampRed::setState(LampRedState nextstate) {
    // block entry for off state
    if (nextstate == LampRedState::Off && currentState != LampRedState::Off) {
        std::cout << "FSMLAMPRED: entry off" << std::endl;
        currentState = LampRedState::Off;
        callbackLR1Off(dispConnectionId);
    }
    // block entry for constant state
    else if (nextstate == LampRedState::Constant && currentState != LampRedState::Constant) {
        std::cout << "FSMLAMPRED: entry on" << std::endl;
        currentState = LampRedState::Constant;
        callbackLR1On(dispConnectionId);
    }
}