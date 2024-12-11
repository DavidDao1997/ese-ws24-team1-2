#include "FSM_HeightMeasurement_Parallel.h"
int8_t FSM_HeightMeasurement_Parallel::numOfPulses = FSM_HEIGHTMEASUREMENT_PARALLEL_NUM_OF_PULSES;
int8_t FSM_HeightMeasurement_Parallel::pulses[FSM_HEIGHTMEASUREMENT_PARALLEL_NUM_OF_PULSES] = {
   PULSE_HS_SAMPLE,
   PULSE_HS_SAMPLING_DONE,
   PULSE_FSM_PARALLEL
};


FSM_HeightMeasurement_Parallel::FSM_HeightMeasurement_Parallel(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    currentState = FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Paused;
}

FSM_HeightMeasurement_Parallel::~FSM_HeightMeasurement_Parallel(){
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    destroyChannel(dispatcherConnectionID);
}

int8_t *FSM_HeightMeasurement_Parallel::getPulses() {
    return pulses;
 }
int8_t FSM_HeightMeasurement_Parallel::getNumOfPulses() {
     return numOfPulses;
}

void FSM_HeightMeasurement_Parallel::handleMsg(){
	_pulse msg;
    running = true;

    while(running){
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);
        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        if(recvid==0){
            int32_t msgVal = msg.value.sival_int;
            switch (msg.code) {
            
            case PULSE_HS_SAMPLE:
                if(currentState == FSM_HEIGHTMEASUREMENT_PARALLELSTATES::PukPresent){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                    }
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_SLOW)) {
                            perror("FSMHeightMeasurement: MOTOR_SLOW failed\n");
                    }
                   
                }else if(currentState == FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Measuring){
                    //TODO
                }
                currentState = FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Measuring;
                std::cout << "FSMHEIGHTMEASUREMENT: Measuring" << std::endl;
                break;
            case PULSE_HS_SAMPLING_DONE:
                if(currentState == FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Measuring){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_SLOW_RESET)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                    }
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, INGRESS_PUK_DISTANCE_VALID)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                    }
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, PUK_ENTRY_SORTING)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                    }
                    currentState = FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Idle;
                    std::cout << "FSMHEIGHTMEASUREMENT: Idle"<< std::endl;

                }
                break;
            case PULSE_FSM_PARALLEL:
                switch(msgVal){
                    case PUK_ENTRY_HeightMeasurement:
                        if(currentState == FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Idle){
                           if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                           } 
                           currentState = FSM_HEIGHTMEASUREMENT_PARALLELSTATES::PukPresent;
                           std::cout << "FSMHEIGHTMEASUREMENT: PukPresent"<< std::endl;

                        }

                        break;
                    case SYSTEM_OPERATIONAL_IN:
                        if(currentState == FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Paused){
                            currentState = FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Idle;
                            std::cout << "FSMHEIGHTMEASUREMENT: Idle"<< std::endl;
                        }
                        break;
                    case SYSTEM_OPERATIONAL_OUT:
                        if(currentState == FSM_HEIGHTMEASUREMENT_PARALLELSTATES::PukPresent){
                           if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                                 perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                             }  
                        } else if(currentState == FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Measuring){
                             if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_SLOW)) {
                                perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                             } 
                        }
                        currentState = FSM_HEIGHTMEASUREMENT_PARALLELSTATES::Paused;
                        std::cout << "FSMHEIGHTMEASUREMENT: Paused"<< std::endl;

                    default:
                        break;
                }
            default:
                break;
            }
        }
    }
}
              



void FSM_HeightMeasurement_Parallel::sendMsg(){}

int32_t FSM_HeightMeasurement_Parallel::getChannel(){
	return channelID;
}
