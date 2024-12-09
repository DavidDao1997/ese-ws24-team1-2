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
    std::cout << "MOTOR GOT ESTOP" << std::endl;
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
        std::cout << "FSMMotor: entry stop" << std::endl;
        callbackMotorStopIn(dispConnectionId);
        currentState = nextState;

    } else if (nextState == Slow && currentState != Slow) {
        std::cout << "FSMMotor: entry slow" << std::endl;
        callbackMotorSlowIn(dispConnectionId);
        currentState = nextState;
    } else if (nextState == Fast && currentState != Fast) {
        std::cout << "FSMMotor: entry fast" << std::endl;
        callbackMotorFastIn(dispConnectionId);
        currentState = Fast;
    }
}