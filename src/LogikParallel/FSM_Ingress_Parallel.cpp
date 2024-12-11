#include "FSM_Ingress_Parallel.h"

int8_t FSM_Ingress_Parallel::numOfPulses = FSM_INGRESS_PARALLEL_NUM_OF_PULSES;
int8_t FSM_Ingress_Parallel::pulses[FSM_INGRESS_PARALLEL_NUM_OF_PULSES] = {
    PULSE_FSM_PARALLEL,
    PULSE_LBF_INTERRUPTED,
    PULSE_LBF_OPEN,
};

FSM_Ingress_Parallel::FSM_Ingress_Parallel(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    currentState = FSM_INGRESS_Parallel_States::Paused;
}

FSM_Ingress_Parallel::~FSM_Ingress_Parallel(){
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    destroyChannel(dispatcherConnectionID);
}

int8_t *FSM_Ingress_Parallel::getPulses() {
    return pulses;
 }
int8_t FSM_Ingress_Parallel::getNumOfPulses() {
     return numOfPulses;
}

void FSM_Ingress_Parallel::handleMsg(){
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
            
            case PULSE_LBF_INTERRUPTED:
                if(currentState == FSM_INGRESS_Parallel_States::Idle){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, INGRESS_OUT)) {
                            perror("FSMINGRESS: INGRESS_OUT failed\n");
                    }
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD)) {
                            perror("FSMINGRESS: MOTOR_FORWARD failed\n");
                    }
                    currentState = FSM_INGRESS_Parallel_States::PukPresent;
                    std::cout << "FSMINGRESS: PukPresent" << std::endl;
                }
                break;
            case PULSE_LBF_OPEN:
                if(currentState == FSM_INGRESS_Parallel_States::PukPresent){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, PUK_ENTRY_HeightMeasurement)) {
                            perror("FSMINGRESS: PUK_ENTRY_HeightMeasurement failed\n");
                    }
                    currentState = FSM_INGRESS_Parallel_States::CreatingDistance;
                    std::cout << "FSMINGRESS: CreatingDistance" << std::endl;
                }
                break;
            case PULSE_FSM_PARALLEL:
                switch(msgVal){
                    case SYSTEM_OPERATIONAL_IN:
                        if(currentState == FSM_INGRESS_Parallel_States::Paused){
                                 if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, INGRESS_IN)) {
                                         perror("FSMINGRESS: INGRESS_IN failed\n");
                                 }
                                currentState = FSM_INGRESS_Parallel_States::Idle;
                                std::cout << "FSMINGRESS: Idle" << std::endl;

                        }
                        break;
                    case SYSTEM_OPERATIONAL_OUT:
                        if(currentState == FSM_INGRESS_Parallel_States::Idle){
                                if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, INGRESS_OUT)) {
                                         perror("FSMINGRESS: INGRESS_OUT failed\n");
                                }
                        }else if(currentState == FSM_INGRESS_Parallel_States::CreatingDistance){
                                if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                                         perror("FSMINGRESS: MOTOR_FORWARD_RESET failed\n");
                                }
                        }
                        currentState = FSM_INGRESS_Parallel_States::Paused;
                        std::cout << "FSMINGRESS: Paused" << std::endl;

                        break;
                    case INGRESS_PUK_DISTANCE_VALID:
                        if(currentState == FSM_INGRESS_Parallel_States::CreatingDistance){
                             if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_FORWARD_RESET)) {
                                         perror("FSMINGRESS: MOTOR_FORWARD_RESET failed\n");
                                }
                             if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, INGRESS_IN)) {
                                         perror("FSMINGRESS: INGRESS_IN failed\n");
                                }
                                currentState = FSM_INGRESS_Parallel_States::Idle;
                                std::cout << "FSMINGRESS: Idle" << std::endl;
                        }
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




void FSM_Ingress_Parallel::sendMsg(){}

int32_t FSM_Ingress_Parallel::getChannel(){
	return channelID;
}
