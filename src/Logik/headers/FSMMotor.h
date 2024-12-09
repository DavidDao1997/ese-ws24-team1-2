#ifndef FSMMOTOR_H
#define FSMMOTOR_H

#include <iostream>
#include <stdint.h>

enum MotorState { Fast, Slow, Stop };

class FSMMotor {
  public:
    FSMMotor(int32_t dispatcherConnectionId);
    ~FSMMotor();

    void raiseSystemEStopIn();
    void raiseSystemEStopOut();
    void raiseIngressPukPresentIn();
    void raiseIngressCreatingDistanceOut();
    void raiseHeightMeasurementPukPresentIn();
    void raiseHeightMeasurementPukPresentOut();
    void raiseHeightMeasurementMeasurementIn();
    void raiseHeightMeasurementMeasurementOut();

    void onMotorStopIn(std::function<void(int32_t conId)> callBackFunction);
    void onMotorFastIn(std::function<void(int32_t conId)> callBackFunction);
    void onMotorSlowIn(std::function<void(int32_t conId)> callBackFunction);

  private:
    int32_t dispConnectionId;
    MotorState currentState;
    void setState(MotorState nextState);
    void evaluteInternalVariables();

    std::function<void(int32_t conId)> callbackMotorStopIn;
    std::function<void(int32_t conId)> callbackMotorFastIn;
    std::function<void(int32_t conId)> callbackMotorSlowIn;

    int8_t motorStopRequests = 0;
    int8_t motorFastRequests = 0;
    int8_t motorSlowRequests = 0;
};

#endif
