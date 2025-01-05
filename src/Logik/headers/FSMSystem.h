#ifndef FSM_SYSTEM_H
#define FSM_SYSTEM_H

#include "../../Logging/headers/Logger.h"

enum SystemState { Start, ServiceMode, Ready, Operational, ESTOP, ESTOP_RESET };

class FSMSystem {
  public:
    FSMSystem(int32_t dispatcherConnectionId);
    ~FSMSystem();

    void onSystemServiceIn(std::function<void(int32_t conId)> callBackFunction);
    void onSystemServiceOut(std::function<void(int32_t conId)> callBackFunction);
    void onEStopIn(std::function<void(int32_t conId)> callBackFunction);
    void onEStopOut(std::function<void(int32_t conId)> callBackFunction);
    void onSystemOperationalIn(std::function<void(int32_t conId)> callBackFunction);
    void onSystemOperationalOut(std::function<void(int32_t conId)> callBackFunction);

    // ESTOP
    void raiseEStop1Low();
    void raiseEStop1High();
    // Start Button
    void raiseBGS1Short();
    void raiseBGS1Long();
    // Stop Button
    void raiseBRS1Short();
    // Reset Button
    void raiseBGR1Short();

    // void raiseInternalValuesChanged(); // What's the plan with that?

  private:
    int32_t dispConnectionId;
    bool calibrated;
    SystemState currentState;

    std::function<void(int32_t conId)> callbackSystemServiceOut;
    std::function<void(int32_t conId)> callbackSystemServiceIn;
    std::function<void(int32_t conId)> callbackEStopIn;
    std::function<void(int32_t conId)> callbackEStopOut;
    std::function<void(int32_t conId)> callbackSystemOperationalIn;
    std::function<void(int32_t conId)> callbackSystemOperationalOut;

    void setState(SystemState nextState);
};

#endif
