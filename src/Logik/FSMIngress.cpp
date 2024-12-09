// FSM_In
#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSMIngress.h"

FSMIngress::FSMIngress(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    setState(FSMIngressStates::Paused);
}

// FSMIngress::~FSMIngress(){}

void FSMIngress::raiseLBF1Interrupted() {
    if (currentState == FSMIngressStates::Idle) {
        setState(FSMIngressStates::PukPresent);
    }
}

void FSMIngress::raiseLBF1Open() {
    if (currentState == FSMIngressStates::PukPresent) {
        setState(FSMIngressStates::CreatingDistance);
    }
}
void FSMIngress::raisePukDistanceValid() {
    if (currentState == FSMIngressStates::CreatingDistance) {
        setState(FSMIngressStates::Idle);
    }
}
void FSMIngress::raiseSystemOperationalIn() {
    if (currentState == FSMIngressStates::Paused) {
        setState(historyState);
    }
}

void FSMIngress::raiseSystemOperationalOut() {
    historyState = currentState;
    setState(FSMIngressStates::Paused);
}

void FSMIngress::onIngressIn(std::function<void(int32_t conId)> callbackFunction) {

    callbackIngressIn = callbackFunction;
}

void FSMIngress::onIngressOut(std::function<void(int32_t conId)> callbackFunction) {
    callbackIngressOut = callbackFunction;
}
void FSMIngress::onPukEntryHeightMeasurement(std::function<void(int32_t conId)> callbackFunction) {
    callbackPukEntryHeightMeasurement = callbackFunction;
}
void FSMIngress::setState(FSMIngressStates nextState) {
    // exit
    if (currentState == FSMIngressStates::Idle && nextState != FSMIngressStates::Idle) {
        callbackIngressIn(dispConnectionId);
    } else if (currentState == FSMIngressStates::CreatingDistance && nextState != FSMIngressStates::CreatingDistance) {
        // motorforward
    }

    // entry
    if (nextState == FSMIngressStates::Idle && currentState != FSMIngressStates::Idle) {
        callbackIngressOut(dispConnectionId);
        currentState = FSMIngressStates::Idle;
    } else if (nextState == FSMIngressStates::PukPresent && currentState != FSMIngressStates::PukPresent) {
        // motorforward
    }
}
