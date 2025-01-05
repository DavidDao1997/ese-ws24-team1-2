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
    Logger::getInstance().log(LogLevel::DEBUG, "BAZZZZZZZZZZZZZZZZZZZZZZZz...", "FSMEgress");
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
        Logger::getInstance().log(LogLevel::DEBUG, "System Operational in...", "FSMEgress");
    }
}
void FSMEgress::raiseSystemOperationalOut() {
    if (currentState != FSMEgressStates::Paused) {
        // historyState = currentState;
        setState(FSMEgressStates::Paused);
        Logger::getInstance().log(LogLevel::DEBUG, "System Operational out...", "FSMEgress");
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
        Logger::getInstance().log(LogLevel::DEBUG, "entry Transfer state...", "FSMEgress");
        callbackEgressTransferIn(dispConnectionId);
        currentState = nextState;
    } else if(currentState != FSMEgressStates::PukPresent && nextState == FSMEgressStates::PukPresent){
        Logger::getInstance().log(LogLevel::DEBUG, "entry PukPresent state...", "FSMEgress");
        currentState = nextState;

        callbackEgressPukPresentIn(dispConnectionId);
    }else if(currentState != FSMEgressStates::Idle && nextState == FSMEgressStates::Idle){
        Logger::getInstance().log(LogLevel::DEBUG, "entry idle...", "FSMEgress");
        currentState = nextState;
    }else{
        currentState = nextState;
    }
}