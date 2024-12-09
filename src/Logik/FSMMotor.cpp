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
        callbackMotorStopIn(dispConnectionId);
        currentState = nextState;

    } else if (nextState == Slow && currentState != Slow) {
        callbackMotorSlowIn(dispConnectionId);
        currentState = nextState;
    } else if (nextState == Fast && currentState != Fast) {
        callbackMotorFastIn(dispConnectionId);
        currentState = Fast;
    }
}