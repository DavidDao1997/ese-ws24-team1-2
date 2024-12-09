#ifndef HEIGHTMEASUREMENT_H
#define HEIGHTMEASUREMENT_H

#include <iostream>
#include <stdint.h>

enum class FSMHeightMeasurementStates { Paused, Idle, PukPresent, Measuring };

class FSMHeightMeasurement {
  public:
    FSMHeightMeasurement(int32_t dispatcherConnectionId);
    ~FSMHeightMeasurement();

    void raisePukEntryHeightMasurement();
    void raiseHS1Sample();
    void raiseHS1SamplingDone();

    void raiseSystemOperationalIn();
    void raiseSystemOperationalOut();

    void onPukDistanceValid(std::function<void(int32_t conId)> callBackFunction);
    void onPukLeaveHeightMeasurement(std::function<void(int32_t conId)> callBackFunction);
    void onMeasurementIn(std::function<void(int32_t conId)> callBackFunction);
    void onMeasurementOut(std::function<void(int32_t conId)> callBackFunction);
    void onPukPresentIn(std::function<void(int32_t conId)> callBackFunction);
    void onPukPresentOut(std::function<void(int32_t conId)> callBackFunction);

  private:
    int32_t dispConnectionId;
    FSMHeightMeasurementStates currentState;
    void setState(FSMHeightMeasurementStates nextState);

    std::function<void(int32_t conId)> callbackPukDistanceValid;
    std::function<void(int32_t conId)> callbackPukLeaveHeightMeasurement;

    std::function<void(int32_t conId)> callbackMeasurementIn;
    std::function<void(int32_t conId)> callbackMeasurementOut;

    std::function<void(int32_t conId)> callbackPukPresentIn;
    std::function<void(int32_t conId)> callbackPukPresentOut;
};

#endif