#include "FSM_Egress_Parallel.h"

int8_t FSM_Egress_Parallel::numOfPulses = FSM_EGRESS_PARALLEL_NUM_OF_PULSES;
int8_t FSM_Egress_Parallel::pulses[FSM_EGRESS_PARALLEL_NUM_OF_PULSES] = {
    PULSE_FSM_PARALLEL,
    PULSE_LBE_INTERRUPTED,
    PULSE_LBE_OPEN,
};

FSM_Egress_Parallel::FSM_Egress_Parallel(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    currentState = FSM_EGRESS_Parallel_States::Paused;
}

FSM_Egress_Parallel::~FSM_Egress_Parallel(){
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    destroyChannel(dispatcherConnectionID);
}

int8_t *FSM_Egress_Parallel::getPulses() {
    return pulses;
 }
int8_t FSM_Egress_Parallel::getNumOfPulses() {
     return numOfPulses;
}

void FSM_Egress_Parallel::handleMsg(){
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
            
            case PULSE_LBE_INTERRUPTED:
                if(currentState == FSM_EGRESS_Parallel_States::PukPresent){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_STOP)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                    } 
                    currentState = FSM_EGRESS_Parallel_States::Transfer;
                    std::cout << "FSMEGRESS: Transfer" << std::endl;

                }
                break;
            case PULSE_LBE_OPEN:
                if(currentState == FSM_EGRESS_Parallel_States::Transfer){
                     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_STOP_RESET)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                        }
                    currentState = FSM_EGRESS_Parallel_States::Idle;
                    std::cout << "FSMEGRESS: Idle" << std::endl;

                }
            case PULSE_FSM_PARALLEL:
                switch(msgVal){
                    case PUK_ENTRY_EGRESS:
                        if(currentState == FSM_EGRESS_Parallel_States::Idle){
                             if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD)) {
                            perror("FSMHeightMeasurement: MOTOR_FORWARD_RESET failed\n");
                            }
                            currentState = FSM_EGRESS_Parallel_States::PukPresent;
                            std::cout << "FSMEGRESS: PukPresent" << std::endl;

                        }
                        break;
                    case SYSTEM_OPERATIONAL_IN:
                        if(currentState == FSM_EGRESS_Parallel_States::Paused){
                            currentState = FSM_EGRESS_Parallel_States::Idle;
                            std::cout << "FSMEGRESS: Idle" << std::endl;

                        }
                        break;
                    case SYSTEM_OPERATIONAL_OUT:
                        if(currentState == FSM_EGRESS_Parallel_States::PukPresent){
                             if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                            perror("FSMEGRESS: MOTOR_FORWARD_RESET failed\n");
                            }
                        }else if(currentState == FSM_EGRESS_Parallel_States::Transfer){
                            if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_STOP_RESET)) {
                            perror("FSMEGRESS: MOTOR_FORWARD_RESET failed\n");
                            }
                        }
                        currentState = FSM_EGRESS_Parallel_States::Paused;
                        std::cout << "FSMEGRESS: Paused" << std::endl;
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



void FSM_Egress_Parallel::sendMsg(){}

int32_t FSM_Egress_Parallel::getChannel(){
	return channelID;
}
