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

  private:
    int32_t dispConnectionId;
    FSMEgressStates currentState;
    FSMEgressStates historyState;
    void setState(FSMEgressStates nextState);
};

#endif
