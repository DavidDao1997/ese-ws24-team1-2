#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSMLampGreen.h"

FSMLampGreen::FSMLampGreen(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    // setState(LampGreenState::Off);
    currentState = LampGreenState::Off;
}

FSMLampGreen::~FSMLampGreen() {}

void FSMLampGreen::raiseSystemServiceIn() {
    if (currentState == LampGreenState::Off) {
        setState(LampGreenState::Blinking1HZ);
    }
}
void FSMLampGreen::raiseSystemServiceOut() {
    if (currentState == LampGreenState::Blinking1HZ) {
        setState(LampGreenState::Off);
    }
}
void FSMLampGreen::raiseEStopCleared() {
    if (currentState == LampGreenState::Constant) {
        setState(LampGreenState::Off);
    }
}
void FSMLampGreen::raiseEStopReceived() {
    if (currentState != LampGreenState::Constant) {
        setState(LampGreenState::Constant);
    }
}
void FSMLampGreen::raiseSystemOperationalIn() {
    if (currentState == LampGreenState::Off) {
        std::cout << "FSMLAMPGREEN: System Operational in" << std::endl;
        setState(LampGreenState::Constant);
    }
}
void FSMLampGreen::raiseSystemOperationalOut() {
    if (currentState == LampGreenState::Constant) {
        std::cout << "FSMLAMPGREEN: System Operational out" << std::endl;

        setState(LampGreenState::Off);
    }
}

void FSMLampGreen::onLG1On(std::function<void(int32_t conId)> callbackFunction) { callbackLG1On = callbackFunction; }
void FSMLampGreen::onLG1Off(std::function<void(int32_t conId)> callbackFunction) { callbackLG1Off = callbackFunction; }
void FSMLampGreen::onLG1Blinking1Hz(std::function<void(int32_t conId)> callbackFunction) {
    callbackLGBlinking1HZ = callbackFunction;
}
// void FSMSystem::onSystemServiceIn(std::function<void()> callbackFunction) {
//     callbackSystemServiceIn = callbackFunction;
// }

void FSMLampGreen::setState(LampGreenState nextState) {

    // Block for entry Off
    if (nextState == LampGreenState::Off && currentState != LampGreenState::Off) {
        std::cout << "FSMLampGreen: entry off" << std::endl;
        currentState = LampGreenState::Off;
        callbackLG1Off(dispConnectionId);
    }
    // Block for entry Constant
    else if (nextState == LampGreenState::Constant && currentState != LampGreenState::Constant) {
        std::cout << "FSMLampGreen: entry on" << std::endl;
        currentState = LampGreenState::Constant;
        callbackLG1On(dispConnectionId);
    }
    // Block for entry Constant
    else if (nextState == LampGreenState::Blinking1HZ && currentState != LampGreenState::Blinking1HZ) {
        std::cout << "FSMLampGreen: entry blinking" << std::endl;
        currentState = LampGreenState::Blinking1HZ;
        callbackLGBlinking1HZ(dispConnectionId);
    }
    // block for entry servicemode
    // if (nextState == ServiceMode && currentState != ServiceMode) {
    //     currentState = ServiceMode;
    //     callbackSystemServiceIn();
    // } // else if { ... }
}
