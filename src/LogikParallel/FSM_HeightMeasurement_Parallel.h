#ifndef FSM_HEIGHTMEASUREMENT_PARALLEL_H
#define FSM_HEIGHTMEASUREMENT_PARALLEL_H

#include "../Dispatcher/headers/PulseMsgConfig.h"
#include "../Dispatcher/headers/PulseMsgHandler.h"
#include "../FSM_QualityGate/src-gen/FSM_QualityGate.h"
#include "../FSM_QualityGate/src/sc_rxcpp.h"

#define FSM_HEIGHTMEASUREMENT_PARALLEL_NUM_OF_PULSES 3
#define FESTO1 0
#define FESTO2 1

enum class FSM_HEIGHTMEASUREMENT_PARALLELSTATES{
    Paused,
    Idle,
    PukPresent,
    Measuring,
};

class FSM_HeightMeasurement_Parallel : public PulseMsgHandler{
    public:
        FSM_HeightMeasurement_Parallel(const std::string dispatcherChannelName);
        virtual ~FSM_HeightMeasurement_Parallel();
        void handleMsg() override;
        void sendMsg() override;
        int32_t getChannel() override;

        int8_t *getPulses();
        int8_t getNumOfPulses();

        void fsmSystemloop();

    private:
        FSM_HEIGHTMEASUREMENT_PARALLELSTATES currentState;
         int32_t channelID;
         int32_t dispatcherConnectionID;
         bool running;
         bool subThreadsRunning;
         static int8_t numOfPulses;
         static int8_t pulses[FSM_HEIGHTMEASUREMENT_PARALLEL_NUM_OF_PULSES];
        
};


#endif
