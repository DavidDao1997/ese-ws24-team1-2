
#ifndef FSM_SYSTEM_PARALLEL_H
#define FSM_SYSTEM_PARALLEL_H


#include "../Dispatcher/headers/PulseMsgConfig.h"
#include "../Dispatcher/headers/PulseMsgHandler.h"
#include "../FSM_QualityGate/src-gen/FSM_QualityGate.h"
#include "../FSM_QualityGate/src/sc_rxcpp.h"

#define FSM_SYSTEM_PARALLEL_NUM_OF_PULSES 6
#define FESTO1 0
#define FESTO2 1

enum class FSM_System_ParallelStates{
    Start,
    ServiceMode,
    Ready,
    Operational,
    EStop,
    ESTOPRESOLVED
};

class FSM_System_Parallel : public PulseMsgHandler{
    public:
        FSM_System_Parallel(const std::string dispatcherChannelName);
        virtual ~FSM_System_Parallel();
        void handleMsg() override;
        void sendMsg() override;
        int32_t getChannel() override;

        int8_t *getPulses();
        int8_t getNumOfPulses();

        void fsmSystemloop();

    private:
        FSM_System_ParallelStates currentState;
         int32_t channelID;
         int32_t dispatcherConnectionID;
         bool calibrated;
         bool running;
         bool subThreadsRunning;
         static int8_t numOfPulses;
         static int8_t pulses[FSM_SYSTEM_PARALLEL_NUM_OF_PULSES];
        
};

#endif
