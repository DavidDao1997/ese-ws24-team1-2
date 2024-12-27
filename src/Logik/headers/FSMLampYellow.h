#ifndef FSMLAMPYELLOW_H
#define FSMLAMPYELLOW_H

#include "../../Logging/headers/Logger.h"

enum class LampYellowState { Off, Blinking1HZ, Constant };

class FSMLampYellow {
  public:
    FSMLampYellow(int32_t dispatcherConnectionId);
    ~FSMLampYellow();

    void raiseIngressIn();
    void raiseIngressOut();

    void raiseEStopCleared();
    void raiseEStopReceived();

    void onLY1On(std::function<void(int32_t conId)> callBackFunction);
    void onLY1Off(std::function<void(int32_t conId)> callBackFunction);
    void onLY1Blinking1Hz(std::function<void(int32_t conId)> callBackFunction);

  private:
    int32_t dispConnectionId;
    LampYellowState currentState;
    std::function<void(int32_t conId)> callbackLY1On;
    std::function<void(int32_t conId)> callbackLY1Off;
    std::function<void(int32_t conId)> callbackLY1Blinking1Hz;
    void setState(LampYellowState nextState);
};

#endif