#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSMSystem.h"

FSMSystem::FSMSystem(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    // setState(Start);
    currentState = Start;
}

FSMSystem::~FSMSystem() {}

void FSMSystem::onSystemServiceIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackSystemServiceIn = callBackFunction;
}
void FSMSystem::onSystemServiceOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackSystemServiceOut = callBackFunction;
}
void FSMSystem::onEStopIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackEStopIn = callBackFunction; }
void FSMSystem::onEStopOut(std::function<void(int32_t conId)> callBackFunction) { 
    callbackEStopOut = callBackFunction; }
    
void FSMSystem::onSystemOperationalIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackSystemOperationalIn = callBackFunction;
}
void FSMSystem::onSystemOperationalOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackSystemOperationalOut = callBackFunction;
}

void FSMSystem::raiseBGS1Long() {
    if (currentState == Start) {
        setState(ServiceMode);
    } else if (currentState == Ready) {
        calibrated = false;
        setState(ServiceMode);
    }
}

void FSMSystem::raiseEStop1Low() {
    if (currentState != ESTOP) {
        std::cout << "FSMSYSTEM: set to EStop" << std::endl;
        setState(ESTOP);
    }
}
void FSMSystem::raiseEStop1High() {
    if (currentState == ESTOP) {
        std::cout << "FSMSYSTEM: set to EStop Reset" << std::endl;
        setState(ESTOP_RESET);
    }
}
void FSMSystem::raiseBGR1Short() {
    if (calibrated && currentState == ESTOP_RESET) {
        setState(Ready);
    } else if (!calibrated && currentState == ESTOP_RESET) {
        setState(Start);
    }
}

void FSMSystem::raiseBGS1Short() {
    if (currentState == Ready) {
        setState(Operational);
    }
}

void FSMSystem::raiseBRS1Short() {
    if (currentState == Operational) {
        setState(Ready);
    } else if (currentState == ServiceMode) {
        calibrated = true;
        setState(Ready);
    }
}

void FSMSystem::setState(SystemState nextState) {
    /*EXIT BLOCKS*/
    // TODO MAKE SURE STATE AwaitingEStopButton doesnt break this
    if (currentState == ESTOP_RESET && nextState != ESTOP_RESET) {
        // exit eStop_reset
        // motorStop = 0
        // motorForward = 0
        // motorSlow = 0
        callbackEStopOut(dispConnectionId);
    } else if (currentState == ServiceMode && nextState != ServiceMode) {
        // exit serviceMode
        callbackSystemServiceOut(dispConnectionId);
    } else if (currentState == Operational && nextState != Operational) {
        callbackSystemOperationalOut(dispConnectionId);
    }

    /*ENTRY BLOCKS*/
    // Entry for EStop
    if (nextState == ESTOP && currentState != ESTOP) {
        std::cout << "FSMSYSTEM: entry eStop " << std::endl;
        // entry eStop
        // motorstop++
        callbackEStopIn(dispConnectionId);
        currentState = ESTOP;
    }
    // Entry for ServiceMode
    else if (nextState == ServiceMode && currentState != ServiceMode) {
        std::cout << "FSMSYSTEM: entry serviceMode " << std::endl;
        // entry servicemode
        currentState = ServiceMode;
        callbackSystemServiceIn(dispConnectionId);
    }
    // Entry for ServiceMode
    else if (nextState == Operational && currentState != Operational) {
        std::cout << "FSMSYSTEM: entry operational " << std::endl;
        currentState = nextState;
        callbackSystemOperationalIn(dispConnectionId);
    } else if (currentState == Ready && nextState == ServiceMode) {
        // ???
        currentState = ServiceMode;
        callbackSystemServiceIn(dispConnectionId);
    } else if (currentState == ESTOP_RESET && nextState != ESTOP_RESET ) {
            std::cout << "FSMSYSTEM: E Stop out" << std::endl;
            currentState = nextState;
            callbackEStopOut(dispConnectionId);

    } else { 
        currentState = nextState;
    }
}