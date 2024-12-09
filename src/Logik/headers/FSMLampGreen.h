#ifndef FSMLAMPGREEN_H
#define FSMLAMPGREEN_H

enum class LampGreenState { Off, Constant, Blinking1HZ };

class FSMLampGreen {
  public:
    FSMLampGreen(int32_t dispatcherConnectionId);
    ~FSMLampGreen();
    void onLG1On(std::function<void(int32_t conId)> callBackFunction);
    void onLG1Off(std::function<void(int32_t conId)> callBackFunction);
    void onLG1Blinking1Hz(std::function<void(int32_t conId)> callBackFunction);
    void raiseSystemServiceIn();
    void raiseSystemServiceOut();
    void raiseEStopReceived();
    void raiseEStopCleared();
    void raiseSystemOperationalIn();
    void raiseSystemOperationalOut();

    //   void onSystemServiceIn(std::function<void()> callBackFunction);
    //   void raiseBGS1LongPressed();

  private:
    int32_t dispConnectionId;
    LampGreenState currentState;
    std::function<void(int32_t conId)> callbackLG1On;
    std::function<void(int32_t conId)> callbackLG1Off;
    std::function<void(int32_t conId)> callbackLGBlinking1HZ;

    void setState(LampGreenState nextState);
};

#endif
