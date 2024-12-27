#include "headers/FSMMotor.h"

FSMMotor::FSMMotor(int32_t dispatcherConnectionId) {
    dispConnectionId = dispatcherConnectionId;
    currentState = Stop;
}

FSMMotor::~FSMMotor() {}

// void FSMMotor::raiseMotorStopRequest() {
//     motorStopRequests++;
//     evaluteInternalVariables();
// }
// void FSMMotor::raiseMotorStopResolve() {
//     motorStopRequests--;
//     evaluteInternalVariables();
// }
// void FSMMotor::raiseMotorFastRequest() {
//     motorFastRequests++;
//     evaluteInternalVariables();
// }
// void FSMMotor::raiseMotorFastResolve() {
//     motorFastRequests--;
//     evaluteInternalVariables();
// }
// void FSMMotor::raiseMotorReset() {
//     motorStopRequests = 0;
//     motorFastRequests = 0;
//     motorSlowRequests = 0;
// }

void FSMMotor::raiseSystemEStopIn() {
    Logger::getInstance().log(LogLevel::DEBUG, "GOT ESTOP...", "FSMMotor");
    motorStopRequests++;
    evaluteInternalVariables();
}
void FSMMotor::raiseSystemEStopOut() {
    motorStopRequests = 0;
    motorFastRequests = 0;
    motorSlowRequests = 0;
    evaluteInternalVariables();
}
void FSMMotor::raiseIngressPukPresentIn() {
    motorFastRequests++;
    evaluteInternalVariables();
}
void FSMMotor::raiseIngressCreatingDistanceOut() {
    motorFastRequests--;
    evaluteInternalVariables();
}

void FSMMotor::raiseHeightMeasurementPukPresentIn() {
    motorFastRequests++;
    evaluteInternalVariables();
}
void FSMMotor::raiseHeightMeasurementPukPresentOut() {
    motorFastRequests--;
    evaluteInternalVariables();
}
void FSMMotor::raiseHeightMeasurementMeasurementIn() {
    motorSlowRequests++;
    evaluteInternalVariables();
}
void FSMMotor::raiseHeightMeasurementMeasurementOut() {
    motorSlowRequests--;
    evaluteInternalVariables();
}

void FSMMotor::raiseSortingPukPresentIn(){
    motorFastRequests++;
    evaluteInternalVariables();
}
void FSMMotor::raiseSortingPukPresentOut(){
    // motorFastRequests--;
    // evaluteInternalVariables();
}
void FSMMotor::raiseSortingMetalMeasurementOut() {
    motorFastRequests--;
    evaluteInternalVariables();
}

void FSMMotor::raiseSortingRampFullIn(){
    motorStopRequests++;
    evaluteInternalVariables();
}

void FSMMotor::raiseSortingRampFullOut(){
    motorStopRequests--;
    evaluteInternalVariables();
}

void FSMMotor::raiseSortingEjectorOut(){
	motorFastRequests--;
    evaluteInternalVariables();
}
void FSMMotor::raiseSortingPassthroughOut(){
    motorFastRequests--;
    evaluteInternalVariables();
}

void FSMMotor::raiseEgressPukPresentIn() {
    motorFastRequests++;
    evaluteInternalVariables();
}

void FSMMotor::raiseEgressPukPresentOut() {
	motorFastRequests--;
    evaluteInternalVariables();
}

void FSMMotor::raiseEgressPukTransferIn() {
    Logger::getInstance().log(LogLevel::DEBUG, "Stop Transfer In...", "FSMMotor");
    motorStopRequests++;
    evaluteInternalVariables();
}

void FSMMotor::raiseEgressPukTransferOut() {
    Logger::getInstance().log(LogLevel::DEBUG, "EgressPukTransferOut...", "FSMMotor");
	motorStopRequests--;
    motorFastRequests--;
    evaluteInternalVariables();
}


void FSMMotor::evaluteInternalVariables() {
    if (motorStopRequests > 0) {
        setState(Stop);
    } else if (motorSlowRequests > 0) {
        setState(Slow);
    } else if (motorFastRequests > 0) {
        setState(Fast);
    } else {
        setState(Stop);
    }
}

void FSMMotor::onMotorStopIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackMotorStopIn = callBackFunction;
}
void FSMMotor::onMotorFastIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackMotorFastIn = callBackFunction;
}
void FSMMotor::onMotorSlowIn(std::function<void(int32_t conId)> callBackFunction) {
    callbackMotorSlowIn = callBackFunction;
}

void FSMMotor::setState(MotorState nextState) {
    if (nextState == Stop && currentState != Stop) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry stop...", "FSMMotor");
        callbackMotorStopIn(dispConnectionId);
        currentState = nextState;

    } else if (nextState == Slow && currentState != Slow) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry slow...", "FSMMotor");
        callbackMotorSlowIn(dispConnectionId);
        currentState = nextState;
    } else if (nextState == Fast && currentState != Fast) {
        Logger::getInstance().log(LogLevel::DEBUG, "entry fast...", "FSMMotor");
        callbackMotorFastIn(dispConnectionId);
        currentState = Fast;
    }
}
