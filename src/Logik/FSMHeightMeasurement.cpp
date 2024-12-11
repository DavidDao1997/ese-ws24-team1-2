// FSM_In
#include <cstdio> // For sprintf
#include <iostream>
#include <string>
#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "../Util/headers/Util.h"
#include "headers/FSMHeightMeasurement.h"

FSMHeightMeasurement::FSMHeightMeasurement(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    // setState(LampGreenState::Off);
    currentState = FSMHeightMeasurementStates::Paused;
}

FSMHeightMeasurement::~FSMHeightMeasurement() {}

void FSMHeightMeasurement::onPukDistanceValid(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukDistanceValid = callBackFunction;
}
void FSMHeightMeasurement::onPukLeaveHeightMeasurement(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukLeaveHeightMeasurement = callBackFunction;
}
void FSMHeightMeasurement::onMeasurementIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackMeasurementIn = callBackFunction;
}
void FSMHeightMeasurement::onMeasurementOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackMeasurementOut = callBackFunction;
}
void FSMHeightMeasurement::onPukPresentIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukPresentIn = callBackFunction;
}
void FSMHeightMeasurement::onPukPresentOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukPresentOut = callBackFunction;
}
void FSMHeightMeasurement::onPukEntrySorting(std::function<void(int32_t conId)> callBackFunction){
    callbackPukEntrySorting = callBackFunction;
}


void FSMHeightMeasurement::raisePukEntryHeightMasurement() {
    if (currentState == FSMHeightMeasurementStates::Idle) {
        setState(FSMHeightMeasurementStates::PukPresent);
    }
}
void FSMHeightMeasurement::raiseHS1Sample() {
    if (currentState == FSMHeightMeasurementStates::PukPresent) {
        setState(FSMHeightMeasurementStates::Measuring);
    }
}
void FSMHeightMeasurement::raiseHS1SamplingDone() {
    if (currentState == FSMHeightMeasurementStates::Measuring) {
        setState(FSMHeightMeasurementStates::Idle);
    }
}

void FSMHeightMeasurement::raiseSystemOperationalIn() {
    if (currentState == FSMHeightMeasurementStates::Paused) {
        std::cout << "FSMHEIGHTMEASUREMENT: System Operational in" << std::endl;
        setState(FSMHeightMeasurementStates::Idle);
    }
}
void FSMHeightMeasurement::raiseSystemOperationalOut() { 
    setState(FSMHeightMeasurementStates::Paused); 
     std::cout << "FSMHEIGHTMEASUREMENT: System Operational out" << std::endl;}

void FSMHeightMeasurement::setState(FSMHeightMeasurementStates nextState) {

    /*Exit Block's*/
    // Exit for PukPresent
    if (currentState == FSMHeightMeasurementStates::PukPresent && nextState != FSMHeightMeasurementStates::PukPresent) {
        callbackPukPresentOut(dispConnectionId); // MotorForward--
    }
    // Exit for Measuring
    else if (currentState == FSMHeightMeasurementStates::Measuring &&
             nextState != FSMHeightMeasurementStates::Measuring) {
        std::cout << "FSMHeightMeasurement: exit measurement " << std::endl;
            callbackMeasurementOut(dispConnectionId);
            
    }

    // Transfer blocks
    if (currentState == FSMHeightMeasurementStates::Measuring && nextState == FSMHeightMeasurementStates::Idle) {
        callbackPukDistanceValid(dispConnectionId);
        callbackPukEntrySorting(dispConnectionId);

    }

    /*Entry Block's*/
    // Entry for PunkPresent
    if (currentState != FSMHeightMeasurementStates::PukPresent && nextState == FSMHeightMeasurementStates::PukPresent) {
        std::cout << "FSMHeightMeasurement: entry PukPresent " << std::endl;
        callbackPukPresentIn(dispConnectionId); // MotorForward++
        currentState = FSMHeightMeasurementStates::PukPresent;
    }
    // Entry for Measuring
    else if (currentState != FSMHeightMeasurementStates::Measuring &&
             nextState == FSMHeightMeasurementStates::Measuring) {
        std::cout << "FSMHeightMeasurement: entry Measuring " << std::endl;
        callbackMeasurementIn(dispConnectionId); // MotorForward++
        currentState = FSMHeightMeasurementStates::Measuring;
    }
    // Entry for Idle
    else if (currentState != FSMHeightMeasurementStates::Idle && nextState == FSMHeightMeasurementStates::Idle) {
        std::cout << "FSMHeightMeasurement: entry Idle " << std::endl;
  
        //callbackPukPresentIn(dispConnectionId);
        currentState = FSMHeightMeasurementStates::Idle;
    } /*Else*/
    else {
        currentState = nextState;
    }
}
