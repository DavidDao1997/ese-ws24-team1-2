#ifndef FSMCONTROLLER_H_
#define FSMCONTROLLER_H_

#include <functional> // For std::bind

#include "../Dispatcher/headers/PulseMsgConfig.h"
#include "../Dispatcher/headers/PulseMsgHandler.h"
#include "src-gen/FSM_QualityGate.h"

#include "../Logging/headers/Logger.h"

#define FSM_CONTROLLER_NUM_OF_PULSES 21

class FSMController : public PulseMsgHandler {
  public:
    FSMController(const std::string dispatcherChannelName);
    virtual ~FSMController();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    int8_t *getPulses();
    int8_t getNumOfPulses();
    bool stop();

  private:
    FSM_QualityGate *fsm;
  
    int32_t channelID;
    int32_t dispatcherConnectionID;

    bool running;
    bool subThreadsRunning;
    static int8_t numOfPulses;
    static int8_t pulses[FSM_CONTROLLER_NUM_OF_PULSES];
};

#endif /* LOGIK_FSM_FSMCONTROLLER_H_ */
