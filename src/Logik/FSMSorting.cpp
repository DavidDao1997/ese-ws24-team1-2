#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSMSorting.h"

FSMSorting::FSMSorting(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    // setState(LampGreenState::Off);
    currentState = FSMSortingStates::Paused;
    rampFull = false;
}

FSMSorting::~FSMSorting() {}

// TODO got a motorFOrward++
void FSMSorting::raisePukEntrySorting() {
    if (currentState == FSMSortingStates::Idle) {
        setState(FSMSortingStates::PukPresent);
    }
}

void FSMSorting::raiseLBM1Interrupted() {
    if (currentState == FSMSortingStates::PukPresent) {
        setState(FSMSortingStates::MetalMeasurement);
    }
}
// TODO got a raise PUK_ENTRY_EGRESS
void FSMSorting::raisePukEjectorDistanceValid() {
    if (currentState == FSMSortingStates::Passthrough) {
        setState(FSMSortingStates::Idle);
    }
}

void FSMSorting::raiseLBR1Open() { rampFull = false; }

void FSMSorting::raiseLBR1Interrupted() { rampFull = true; }

void FSMSorting::raiseBGR1Interrupted() {
    if (currentState == FSMSortingStates::RampFull) {
        setState(FSMSortingStates::Ejecting);
    }
}

void FSMSorting::raiseSystemOperationalIn() {
    if (currentState == FSMSortingStates::Paused) {
        std::cout << "FSMSORTING: System Operational in" << std::endl;
        setState(FSMSortingStates::Idle);
    }
}
void FSMSorting::raiseSystemOperationalOut() {
     setState(FSMSortingStates::Paused); 
     std::cout << "FSMSORTING: System Operational out" << std::endl;
}


//callback's method's

void FSMSorting::onPukPresentIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukPresentIn = callBackFunction;
}
void FSMSorting::onPukPresentOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukPresentOut = callBackFunction;
}

void FSMSorting::onPukEntryEgress(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukEntryEgress = callBackFunction;
}
//can be ejected
void FSMSorting::onPukEjectorDistanceValid(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukEjectorDistanceValid = callBackFunction;
}

void FSMSorting::onRampFullIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackRampFullIn = callBackFunction;
}
void FSMSorting::onRampFullOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackRampFullOut = callBackFunction;
}
void FSMSorting::onPassthroughOut(std::function<void(int32_t conId)> callBackFunction){
    callbackPassthroughOut = callBackFunction;
}

void FSMSorting::onEjectingOut(std::function<void(int32_t conId)> callBackFunction){
    callbackEjectingOut = callBackFunction;
}
//On LBM so that Metal Measurement can be read 
void FSMSorting::onMetalMeasurementIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackMetalMeasurementIn = callBackFunction;
}
void FSMSorting::onMetalMeasurementOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackMetalMeasurementOut = callBackFunction;
}



void FSMSorting::setState(FSMSortingStates nextState) {
    /*EXIT BLock*/
    // Exit for Idle
    if (currentState == FSMSortingStates::Idle && nextState != FSMSortingStates::Idle) {
        //callbackPukPresentIn(dispConnectionId);
    }
    // Exit for Measuring but how for which one
    else if (currentState == FSMSortingStates::MetalMeasurement && nextState != FSMSortingStates::MetalMeasurement) {
        callbackMetalMeasurementOut(dispConnectionId);
    }
    // Exit for Passthrough
    else if (currentState == FSMSortingStates::Passthrough && nextState != FSMSortingStates::Passthrough) {
        callbackPukEntryEgress(dispConnectionId);
        // TODO MotorForward--
    }
    // Exit for Rampfull
    else if (currentState == FSMSortingStates::RampFull && nextState != FSMSortingStates::RampFull) {
        callbackRampFullOut(dispConnectionId); // MotorForward--
        // rampFull = false;
    }else if(currentState == FSMSortingStates::PukPresent && nextState != FSMSortingStates::PukPresent){
            callbackPukPresentOut(dispConnectionId);
    }

    /*ENTRY Block*/
    if (currentState != FSMSortingStates::RampFull && nextState == FSMSortingStates::RampFull) {
        std::cout << "FSMSorting: entry RampFull " << std::endl;
        callbackRampFullIn(dispConnectionId); // MotorForward++
        currentState = FSMSortingStates::RampFull;
    } else if (currentState != FSMSortingStates::MetalMeasurement && nextState == FSMSortingStates::MetalMeasurement) {
        callbackMetalMeasurementIn(dispConnectionId);
    }
    else if (currentState != FSMSortingStates::Ejecting && nextState == FSMSortingStates::Ejecting) {
        callbackPukEjectorDistanceValid(dispConnectionId);
    }  
    else if (currentState != FSMSortingStates::Idle && nextState == FSMSortingStates::Idle) {
        //callbackPukPresentIn(dispConnectionId);
    }else if(currentState != FSMSortingStates::PukPresent && nextState == FSMSortingStates::PukPresent){
        callbackPukPresentIn(dispConnectionId);
    }

    else {
        currentState = nextState;
    }
}
