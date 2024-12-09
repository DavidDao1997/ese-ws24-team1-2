// FSM_Egress
#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSMEgress.h"

FSMEgress::FSMEgress(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    currentState = FSMEgressStates::Paused;
    // setState(FSMEgressStates::Paused);
}

FSMEgress::~FSMEgress() {}

void FSMEgress::raisePukEntryEgress() {
    if (currentState == FSMEgressStates::Idle) {
        setState(FSMEgressStates::PukPresent);
    }
}
void FSMEgress::raiseLBE1Interrupted() {
    if (currentState == FSMEgressStates::PukPresent) {
        setState(FSMEgressStates::Transfer);
    }
}

void FSMEgress::raiseLBE1Open() {
    if (currentState == FSMEgressStates::Transfer) {
        setState(FSMEgressStates::Idle);
    }
}

void FSMEgress::raiseSystemOperationalIn() {
    if (currentState == FSMEgressStates::Paused) {
        // setState(historyState);
        setState(FSMEgressStates::Idle);
    }
}
void FSMEgress::raiseSystemOperationalOut() {
    if (currentState != FSMEgressStates::Paused) {
        // historyState = currentState;
        setState(FSMEgressStates::Paused);
    }
}

void FSMEgress::setState(FSMEgressStates nextState) {
    // block exit
    if (currentState == FSMEgressStates::Transfer && nextState != FSMEgressStates::Transfer) {
        // TODO Motor--
    }
    // block Entry
    if (currentState != FSMEgressStates::Transfer && nextState == FSMEgressStates::Transfer) {
        std::cout << "FSMEgress: entry Transfer " << std::endl;
        // TODO motorStop++
        currentState = FSMEgressStates::Transfer;
    } else {
        currentState = nextState;
    }
}