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
    // setState(FSMIngressStates::Paused);
    currentState = FSMIngressStates::Paused;
}

FSMIngress::~FSMIngress() {}

/* Events */
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
        // setState(historyState);
        std::cout << "FSMINGRESS: System Operational in" << std::endl;
        setState(FSMIngressStates::Idle);
    }
}

void FSMIngress::raiseSystemOperationalOut() {
    // historyState = currentState;
    std::cout << "FSMINGRESS: System Operational out" << std::endl;
    setState(FSMIngressStates::Paused);
}
// callback function/exit
void FSMIngress::onIngressIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackIngressIn = callBackFunction;
}

void FSMIngress::onIngressOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackIngressOut = callBackFunction;
}
void FSMIngress::onPukEntryHeightMeasurement(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukEntryHeightMeasurement = callBackFunction;
}

void FSMIngress::onPukPresentIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukPresentIn = callBackFunction;
}

void FSMIngress::onPukCreatingDistanceOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukCreatingDistanceOut = callBackFunction;
}

// set the next state
void FSMIngress::setState(FSMIngressStates nextState) {
    // exit
    if (currentState == FSMIngressStates::Idle && nextState != FSMIngressStates::Idle) {
        callbackIngressIn(dispConnectionId);
    } else if (currentState == FSMIngressStates::CreatingDistance && nextState != FSMIngressStates::CreatingDistance) {
        callbackPukCreatingDistanceOut(dispConnectionId); // MotorForward--
    }

    // transitions
    if (currentState == FSMIngressStates::PukPresent && nextState == FSMIngressStates::CreatingDistance) {
        callbackPukEntryHeightMeasurement(dispConnectionId);
    }

    // entry
    if (nextState == FSMIngressStates::Idle && currentState != FSMIngressStates::Idle) {
        std::cout << "FSMIngresss: entry idle " << std::endl;
        callbackIngressOut(dispConnectionId);
        currentState = nextState;
    } else if (nextState == FSMIngressStates::PukPresent && currentState != FSMIngressStates::PukPresent) {
        // entry PukPresent
        std::cout << "FSMIngresss: entry PukPresent " << std::endl;
        callbackPukPresentIn(dispConnectionId); // MotorForward++;
        currentState = FSMIngressStates::PukPresent;
    } else if (nextState == FSMIngressStates::CreatingDistance && currentState != FSMIngressStates::CreatingDistance) {
        std::cout << "FSMIngresss: entry CreatingDistance " << std::endl;
        currentState = nextState;
    } else {
        currentState = nextState;
    }
}
