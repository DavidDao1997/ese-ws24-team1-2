
#ifndef FSM_MOTOR_PARALLEL_H
#define FSM_MOTOR_PARALLEL_H


#include "../Dispatcher/headers/PulseMsgConfig.h"
#include "../Dispatcher/headers/PulseMsgHandler.h"
#include "../FSM_QualityGate/src-gen/FSM_QualityGate.h"
#include "../FSM_QualityGate/src/sc_rxcpp.h"

#define FSM_MOTOR_PARALLEL_NUM_OF_PULSES 3
#define FESTO1 0
#define FESTO2 1

enum class FSM_MOTOR_Parallel_States{
    Paused,
    Idle,
    PukPresent,
    Transfer,
};

class FSM_Motor_Parallel : public PulseMsgHandler{
    public:
	FSM_Motor_Parallel(const std::string dispatcherChannelName);
        virtual ~FSM_Motor_Parallel();
        void handleMsg() override;
        void sendMsg() override;
        int32_t getChannel() override;

        int8_t *getPulses();
        int8_t getNumOfPulses();
        void evaluateMotor();

    private:


         int32_t channelID;

         bool running;
         bool subThreadsRunning;
         static int8_t numOfPulses;
         static int8_t pulses[FSM_MOTOR_PARALLEL_NUM_OF_PULSES];

};

#endif
