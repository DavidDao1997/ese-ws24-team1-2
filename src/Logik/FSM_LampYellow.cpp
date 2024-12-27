#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSMLampYellow.h"

FSMLampYellow::FSMLampYellow(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    // setState(LampYellowState::Off);
    currentState = LampYellowState::Off;
}

FSMLampYellow::~FSMLampYellow() {}

void FSMLampYellow::raiseIngressIn() {
    if (currentState == LampYellowState::Off) {
        setState(LampYellowState::Blinking1HZ);
    }
}
void FSMLampYellow::raiseIngressOut() {
    if (currentState == LampYellowState::Blinking1HZ) {
        setState(LampYellowState::Off);
    }
}

void FSMLampYellow::raiseEStopReceived() {
    if (currentState == LampYellowState::Blinking1HZ) {
        setState(LampYellowState::Constant);
    } else if (currentState == LampYellowState::Off) {
        Logger::getInstance().log(LogLevel::DEBUG, "ESTOP Received...", "FSMLampYellow");
        setState(LampYellowState::Constant);
    }
}

void FSMLampYellow::raiseEStopCleared() {
    if (currentState == LampYellowState::Constant) {
        Logger::getInstance().log(LogLevel::DEBUG, "ESTOP Cleared...", "FSMLampYellow");
        setState(LampYellowState::Off);
    }
}

void FSMLampYellow::onLY1On(std::function<void(int32_t conId)> callbackFunction) { callbackLY1On = callbackFunction; }
void FSMLampYellow::onLY1Off(std::function<void(int32_t conId)> callbackFunction) { callbackLY1Off = callbackFunction; }
void FSMLampYellow::onLY1Blinking1Hz(std::function<void(int32_t conId)> callbackFunction) {
    callbackLY1Blinking1Hz = callbackFunction;
}
// void FSMSystem::onSystemServiceIn(std::function<void()> callbackFunction) {
//     callbackSystemServiceIn = callbackFunction;
// }

void FSMLampYellow::setState(LampYellowState nextState) {
    if (nextState == LampYellowState::Off && currentState != LampYellowState::Off) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry off...", "FSMLampYellow");
        currentState = LampYellowState::Off;
        callbackLY1Off(dispConnectionId);
    }
    // Block for entry Constant
    else if (nextState == LampYellowState::Constant && currentState != LampYellowState::Constant) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry on...", "FSMLampYellow");
        currentState = LampYellowState::Constant;
        callbackLY1On(dispConnectionId);
    }
    // Block for entry Constant
    else if (nextState == LampYellowState::Blinking1HZ && currentState != LampYellowState::Blinking1HZ) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry blinking...", "FSMLampYellow");
        currentState = LampYellowState::Blinking1HZ;
        callbackLY1Blinking1Hz(dispConnectionId);
    }
}
