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
        Logger::getInstance().log(LogLevel::DEBUG, "ESTOP Received...", "FSMLampRed");
        setState(LampRedState::Constant);
    }
}
void FSMLampRed::raiseEStopCleared() {
    if (currentState == LampRedState::Constant) {
        Logger::getInstance().log(LogLevel::DEBUG, "ESTOP Cleared...", "FSMLampRed");
        setState(LampRedState::Off);
    }
}

void FSMLampRed::setState(LampRedState nextstate) {
    // block entry for off state
    if (nextstate == LampRedState::Off && currentState != LampRedState::Off) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry off...", "FSMLampRed");
        currentState = LampRedState::Off;
        callbackLR1Off(dispConnectionId);
    }
    // block entry for constant state
    else if (nextstate == LampRedState::Constant && currentState != LampRedState::Constant) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry on...", "FSMLampRed");
        currentState = LampRedState::Constant;
        callbackLR1On(dispConnectionId);
    }
}