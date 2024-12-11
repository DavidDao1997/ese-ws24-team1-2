
#ifndef FSM_INGRESS_PARALLEL_H
#define FSM_INGRESS_PARALLEL_H


#include "../Dispatcher/headers/PulseMsgConfig.h"
#include "../Dispatcher/headers/PulseMsgHandler.h"
#include "../FSM_QualityGate/src-gen/FSM_QualityGate.h"
#include "../FSM_QualityGate/src/sc_rxcpp.h"

#define FSM_INGRESS_PARALLEL_NUM_OF_PULSES 3
#define FESTO1 0
#define FESTO2 1

enum class FSM_INGRESS_Parallel_States{
    Paused,
    Idle,
    PukPresent,
    CreatingDistance,
};

class FSM_Ingress_Parallel : public PulseMsgHandler{
    public:
	FSM_Ingress_Parallel(const std::string dispatcherChannelName);
        virtual ~FSM_Ingress_Parallel();
        void handleMsg() override;
        void sendMsg() override;
        int32_t getChannel() override;

        int8_t *getPulses();
        int8_t getNumOfPulses();

        void fsmSystemloop();

    private:
        FSM_INGRESS_Parallel_States currentState;
         int32_t channelID;
         int32_t dispatcherConnectionID;
         bool rampFull;
         bool running;
         bool subThreadsRunning;
         static int8_t numOfPulses;
         static int8_t pulses[FSM_INGRESS_PARALLEL_NUM_OF_PULSES];

};

#endif
