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
        std::cout << "FSMEGRESS: System Operational in" << std::endl;

    }
}
void FSMEgress::raiseSystemOperationalOut() {
    if (currentState != FSMEgressStates::Paused) {
        // historyState = currentState;
        setState(FSMEgressStates::Paused);
        std::cout << "FSMEGRESS: System Operational out" << std::endl;

    }
}

void FSMEgress::onEgressTransferIn(std::function<void(int32_t conId)> callBackFunction){
	callbackEgressTransferIn = callBackFunction;
}
void FSMEgress::onEgressTransferOut(std::function<void(int32_t conId)> callBackFunction){
	callbackEgressTransferOut = callBackFunction;
}
void FSMEgress::onEgressPukPresentIn(std::function<void(int32_t conId)> callBackFunction){
	callbackEgressPukPresentIn = callBackFunction;
}
void FSMEgress::onEgressPukPresentOut(std::function<void(int32_t conId)> callBackFunction){
	callbackEgressPukPresentOut = callBackFunction;
}

void FSMEgress::setState(FSMEgressStates nextState) {
    // block exit
    if (currentState == FSMEgressStates::Transfer && nextState != FSMEgressStates::Transfer) {
       callbackEgressTransferOut(dispConnectionId);
    }else if(currentState == FSMEgressStates::PukPresent && nextState != FSMEgressStates::PukPresent){
        callbackEgressPukPresentOut(dispConnectionId);
    }
    // block Entry
    if (currentState != FSMEgressStates::Transfer && nextState == FSMEgressStates::Transfer) {
        std::cout << "FSMEgress: entry Transfer state " << std::endl;
        callbackEgressTransferIn(dispConnectionId);
        currentState = nextState;
    } else if(currentState != FSMEgressStates::PukPresent && nextState == FSMEgressStates::PukPresent){
        currentState = nextState;
        callbackEgressPukPresentIn(dispConnectionId);
    }else{
        currentState = nextState;
    }
}