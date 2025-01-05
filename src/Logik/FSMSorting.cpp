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
    Logger::getInstance().log(LogLevel::DEBUG, "FOOOOOOOOOO...", "FSMSorting");
    if (currentState == FSMSortingStates::Idle) {
        setState(FSMSortingStates::PukPresent);
    }
}

void FSMSorting::raiseLBM1Interrupted() { 
    if (currentState == FSMSortingStates::PukPresent && !metalDetected) {
        setState(FSMSortingStates::Passthrough);
    } else if (currentState == FSMSortingStates::PukPresent && metalDetected && rampFull) {
        setState(FSMSortingStates::RampFull);
    } else if (currentState == FSMSortingStates::PukPresent && metalDetected && !rampFull ) {
        setState(FSMSortingStates::Ejecting);
    }
}
void FSMSorting::raiseLBM1Open() {
    if (currentState == FSMSortingStates::Passthrough || currentState == FSMSortingStates::Ejecting) {
        setState(FSMSortingStates::Idle);
    }
}
void FSMSorting::raiseMSTrue() {
    metalDetected = true;
}

void FSMSorting::raiseMSFalse() {
    metalDetected = false;
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
    Logger::getInstance().log(LogLevel::DEBUG, "BARRRRRRRRRR...", "FSMSorting");
    if (currentState == FSMSortingStates::Paused) {
        Logger::getInstance().log(LogLevel::DEBUG, "System Operational in 3...", "FSMSorting");
        setState(FSMSortingStates::Idle);
    }
}
void FSMSorting::raiseSystemOperationalOut() {
    setState(FSMSortingStates::Paused);
    Logger::getInstance().log(LogLevel::DEBUG, "System Operational out...", "FSMSorting");
}

// callback's method's

void FSMSorting::onPukPresentIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukPresentIn = callBackFunction;
}
void FSMSorting::onPukPresentOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukPresentOut = callBackFunction;
}

void FSMSorting::onPukEntryEgress(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukEntryEgress = callBackFunction;
}
// can be ejected
void FSMSorting::onPukEjectorDistanceValid(std::function<void(int32_t conId)> callBackFunction) {
    callbackPukEjectorDistanceValid = callBackFunction;
}

void FSMSorting::onRampFullIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackRampFullIn = callBackFunction;
}
void FSMSorting::onRampFullOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackRampFullOut = callBackFunction;
}
void FSMSorting::onPassthroughOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackPassthroughOut = callBackFunction;
}

void FSMSorting::onEjectingOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackEjectingOut = callBackFunction;
}
// On LBM so that Metal Measurement can be read
void FSMSorting::onMetalMeasurementIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackMetalMeasurementIn = callBackFunction;
}
void FSMSorting::onMetalMeasurementOut(std::function<void(int32_t conId)> callBackFunction) {
    callbackMetalMeasurementOut = callBackFunction;
}
void FSMSorting::onSortingModuleActive(std::function<void(int32_t conId)> callBackFunction) {
    callbackSortingModuleActive = callBackFunction;
}
void FSMSorting::onSortingModuleResting(std::function<void(int32_t conId)> callBackFunction) {
    callbackSortingModuleResting = callBackFunction;
}

void FSMSorting::setState(FSMSortingStates nextState) {
    /*EXIT BLock*/
    // Exit for Idle
    if (currentState == FSMSortingStates::Idle && nextState != FSMSortingStates::Idle) {
        // callbackPukPresentIn(dispConnectionId);
    }
    // Exit for Measuring but how for which one
    else if (currentState == FSMSortingStates::MetalMeasurement && nextState != FSMSortingStates::MetalMeasurement) {
        // callbackMetalMeasurementOut(dispConnectionId);
        // callbackPukEntryEgress(dispConnectionId);
    }
    // Exit for Passthrough
    else if (currentState == FSMSortingStates::Passthrough && nextState != FSMSortingStates::Passthrough) {
        // callbackPukEntryEgress(dispConnectionId);
        // TODO MotorForward--
    }
    // Exit for Rampfull
    else if (currentState == FSMSortingStates::RampFull && nextState != FSMSortingStates::RampFull) {
        callbackRampFullOut(dispConnectionId); // MotorForward--
        rampFull = false;
    } else if (currentState == FSMSortingStates::PukPresent && nextState != FSMSortingStates::PukPresent) {
        // callbackPukPresentOut(dispConnectionId);
    }

    Logger::getInstance().log(LogLevel::DEBUG, "ALMOST EEEEEEEEEEEJJJJJECTIIIIINNNNNNNNNGGGGGGGGGGG...", "FSMSorting");
    /*TRANSFER Block*/
    if(currentState == FSMSortingStates::Passthrough && nextState == FSMSortingStates::Idle) {
        Logger::getInstance().log(LogLevel::DEBUG, "EEEEEEEEEEEJJJJJECTIIIIINNNNNNNNNGGGGGGGGGGG...", "FSMSorting");
        callbackPassthroughOut(dispConnectionId);
    } else if(currentState == FSMSortingStates::Ejecting && nextState == FSMSortingStates::Idle) {
        Logger::getInstance().log(LogLevel::DEBUG, "EEEEEEEEEEEJJJJJECTIIIIINNNNNNNNNGGGGGGGGGGG...", "FSMSorting");
        callbackEjectingOut(dispConnectionId);
    } else if (currentState == FSMSortingStates::PukPresent && nextState == FSMSortingStates::Passthrough) {
        callbackPukEntryEgress(dispConnectionId);
    } else if (currentState == FSMSortingStates::PukPresent && nextState == FSMSortingStates::Ejecting) {
        callbackSortingModuleActive(dispConnectionId);
        WAIT(500);
        callbackSortingModuleResting(dispConnectionId);
    } else if (currentState == FSMSortingStates::RampFull && nextState == FSMSortingStates::Ejecting) {
        callbackSortingModuleActive(dispConnectionId);
        WAIT(500);
        callbackSortingModuleResting(dispConnectionId);
    } 

    /*ENTRY Block*/
    if (currentState != FSMSortingStates::RampFull && nextState == FSMSortingStates::RampFull) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry RampFull...", "FSMSorting");
        callbackRampFullIn(dispConnectionId); // MotorForward++
        currentState = nextState;
    } else if (currentState != FSMSortingStates::MetalMeasurement && nextState == FSMSortingStates::MetalMeasurement) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry metalMeasurement...", "FSMSorting");
        // callbackPukEntryEgress(dispConnectionId);
        // callbackMetalMeasurementIn(dispConnectionId);
        currentState = nextState;
    } else if (currentState != FSMSortingStates::Ejecting && nextState == FSMSortingStates::Ejecting) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry ejecting...", "FSMSorting");
        // callbackPukEjectorDistanceValid(dispConnectionId);
        currentState = nextState;
    } else if (currentState != FSMSortingStates::Idle && nextState == FSMSortingStates::Idle) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry idle...", "FSMSorting");
        currentState = nextState;
    } else if (currentState != FSMSortingStates::PukPresent && nextState == FSMSortingStates::PukPresent) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry pukPresent...", "FSMSorting");
        callbackPukPresentIn(dispConnectionId);
        currentState = nextState;
    } else if (currentState != FSMSortingStates::Passthrough && nextState == FSMSortingStates::Passthrough) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry passthrough...", "FSMSorting");
        currentState = nextState;
    } else {
        currentState = nextState;
    }
}
