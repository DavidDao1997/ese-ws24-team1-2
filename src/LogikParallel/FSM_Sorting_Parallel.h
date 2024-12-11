
#ifndef FSM_SORTING_PARALLEL_H
#define FSM_SORTING_PARALLEL_H


#include "../Dispatcher/headers/PulseMsgConfig.h"
#include "../Dispatcher/headers/PulseMsgHandler.h"
#include "../FSM_QualityGate/src-gen/FSM_QualityGate.h"
#include "../FSM_QualityGate/src/sc_rxcpp.h"

#define FSM_SORTING_PARALLEL_NUM_OF_PULSES 5
#define FESTO1 0
#define FESTO2 1

enum class FSM_SORTING_Parallel_States{
    Paused,
    Idle,
    PukPresent,
    MetalMeasurement,
    Ejecting,
    RampFull,  
    Passthrough
};

class FSM_Sorting_Parallel : public PulseMsgHandler{
    public:
	FSM_Sorting_Parallel(const std::string dispatcherChannelName);
        virtual ~FSM_Sorting_Parallel();
        void handleMsg() override;
        void sendMsg() override;
        int32_t getChannel() override;

        int8_t *getPulses();
        int8_t getNumOfPulses();

        void fsmSystemloop();

    private:
        FSM_SORTING_Parallel_States currentState;
         int32_t channelID;
         int32_t dispatcherConnectionID;
        bool rampFull;
         bool running;
         bool subThreadsRunning;
         static int8_t numOfPulses;
         static int8_t pulses[FSM_SORTING_PARALLEL_NUM_OF_PULSES];

};

#endif
