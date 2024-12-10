#ifndef FSMEGRESS_H
#define FSMEGRESS_H

enum class FSMEgressStates { Paused, Idle, Transfer, PukPresent };

class FSMEgress {
  public:
    FSMEgress(int32_t dispatcherConnectionId);
    ~FSMEgress();

    void raisePukEntryEgress();
    void raiseLBE1Interrupted();
    void raiseLBE1Open();
    void raiseSystemOperationalIn();
    void raiseSystemOperationalOut();
    
    void onEgressTransferIn(std::function<void(int32_t conId)> callBackFunction);
    void onEgressTransferOut(std::function<void(int32_t conId)> callBackFunction);
    void onEgressPukPresentIn(std::function<void(int32_t conId)> callBackFunction);
    void onEgressPukPresentOut(std::function<void(int32_t conId)> callBackFunction);

  private:
    int32_t dispConnectionId;
    FSMEgressStates currentState;
    FSMEgressStates historyState;

    std::function<void(int32_t conId)> callbackEgressPukPresentIn;
  	std::function<void(int32_t conId)> callbackEgressPukPresentOut;
    std::function<void(int32_t conId)> callbackEgressTransferIn;
  	std::function<void(int32_t conId)> callbackEgressTransferOut;
    void setState(FSMEgressStates nextState);
};

#endif
