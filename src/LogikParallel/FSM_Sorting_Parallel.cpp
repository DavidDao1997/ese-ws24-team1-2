#include "FSM_Sorting_Parallel.h"

int8_t FSM_Sorting_Parallel::numOfPulses = FSM_SORTING_PARALLEL_NUM_OF_PULSES;
int8_t FSM_Sorting_Parallel::pulses[FSM_SORTING_PARALLEL_NUM_OF_PULSES] = {
    PULSE_FSM_PARALLEL,
    PULSE_LBM_INTERRUPTED,
    PULSE_LBR_OPEN,
    PULSE_LBR_INTERRUPTED,
    PULSE_BGR_SHORT
};

FSM_Sorting_Parallel::FSM_Sorting_Parallel(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    currentState = FSM_SORTING_Parallel_States::Paused;
}

FSM_Sorting_Parallel::~FSM_Sorting_Parallel(){
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    destroyChannel(dispatcherConnectionID);
}

int8_t *FSM_Sorting_Parallel::getPulses() {
    return pulses;
 }
int8_t FSM_Sorting_Parallel::getNumOfPulses() {
     return numOfPulses;
}

void FSM_Sorting_Parallel::handleMsg(){
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
                case PULSE_LBM_INTERRUPTED:
                    if(currentState == FSM_SORTING_Parallel_States::PukPresent){
                        currentState = FSM_SORTING_Parallel_States::MetalMeasurement;
                        std::cout << "FSMSORTING: MetalMeasurement" << std::endl;

                        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, PUK_ENTRY_EGRESS)) {
                                    perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                                }
                        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                                    perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                                }
                        currentState = FSM_SORTING_Parallel_States::Idle;
                        std::cout << "FSMSORTING: Idle" << std::endl;

                        //TODO: Measuring Auswertung
                    }
                    break;
                case PULSE_LBR_OPEN:
                    rampFull = false;
                    break;
                case PULSE_LBR_INTERRUPTED:
                    rampFull = true;
                    break;
                case PULSE_BGR_SHORT:
                    if(currentState == FSM_SORTING_Parallel_States::RampFull){
                        rampFull = false;
                         if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_STOP_RESET)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                             }
                        currentState = FSM_SORTING_Parallel_States::Ejecting;
                        std::cout << "FSMSORTING: Ejecting" << std::endl;

                    }
                    break;
                case PULSE_FSM_PARALLEL:
                    switch(msgVal){
                    
                    case PUK_ENTRY_SORTING:
                        if(currentState == FSM_SORTING_Parallel_States::Idle){
                             if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD)) {
                                 perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                                }
                            currentState = FSM_SORTING_Parallel_States::PukPresent;
                            std::cout << "FSMSORTING: PukPresent" << std::endl;

                        }
                        break;
                    case SYSTEM_OPERATIONAL_IN:
                        if(currentState == FSM_SORTING_Parallel_States::Paused){
                            currentState = FSM_SORTING_Parallel_States::Idle;
                            std::cout << "FSMSORTING: Idle" << std::endl;

                        }
                        break;
                    case SYSTEM_OPERATIONAL_OUT:
                        if(currentState == FSM_SORTING_Parallel_States::RampFull){
                             if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_STOP_RESET)) {
                                    perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                                }
                            
                        }else if(currentState==FSM_SORTING_Parallel_States::PukPresent){
                            if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                                    perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                                }
                        }else if(currentState == FSM_SORTING_Parallel_States::Passthrough){
                            if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                                    perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                                }
                        }
                        currentState = FSM_SORTING_Parallel_States::Paused;
                        std::cout << "FSMSORTING: Paused" << std::endl;

                        break;
                    default:
                    	break;

                    }
                default:
                	break;



             }
        }
    }
}




void FSM_Sorting_Parallel::sendMsg(){}

int32_t FSM_Sorting_Parallel::getChannel(){
	return channelID;
}
