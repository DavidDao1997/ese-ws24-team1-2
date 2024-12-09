#ifndef FSMINGRESS_H
#define FSMINGRESS_H

enum class FSMIngressStates { Paused, Idle, CreatingDistance, PukPresent };

class FSMIngress {
  public:
    FSMIngress(int32_t dispatcherConnectionId);
    ~FSMIngress() {};

    void raisePukDistanceValid();
    void raiseLBF1Interrupted();
    void raiseLBF1Open();
    void raiseSystemOperationalIn();
    void raiseSystemOperationalOut();

    void onPukEntryHeightMeasurement(std::function<void(int32_t conId)> callBackFunction);
    void onIngressOut(std::function<void(int32_t conId)> callBackFunction);
    void onIngressIn(std::function<void(int32_t conId)> callBackFunction);

  private:
    int32_t dispConnectionId;
    FSMIngressStates currentState;
    FSMIngressStates historyState;
    std::function<void(int32_t conId)> callbackPukEntryHeightMeasurement;
    std::function<void(int32_t conId)> callbackIngressIn;
    std::function<void(int32_t conId)> callbackIngressOut;
    void setState(FSMIngressStates nextState);
};

#endif
