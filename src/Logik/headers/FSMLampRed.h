#ifndef FSMLAMPRED_H
#define FSMLAMPRED_H

#include "../../Logging/headers/Logger.h"

enum class LampRedState { Off, Constant };

class FSMLampRed {

  public:
    FSMLampRed(int32_t dispatcherConnectionId);
    ~FSMLampRed();

    void onLR1On(std::function<void(int32_t conId)> callBackFunction);
    void onLR1Off(std::function<void(int32_t conId)> callBackFunction);

    void raiseEStopReceived();
    void raiseEStopCleared();

  private:
    int32_t dispConnectionId;
    LampRedState currentState;

    std::function<void(int32_t conId)> callbackLR1On;
    std::function<void(int32_t conId)> callbackLR1Off;

    void setState(LampRedState nextState);
};

#endif
