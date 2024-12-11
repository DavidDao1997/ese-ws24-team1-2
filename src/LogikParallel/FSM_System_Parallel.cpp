/*
 * FSM_System_Parallel.cpp
 *
 *  Created on: 11.12.2024
 *      Author: Jannik
 */

#include "FSM_System_Parallel.h"

int8_t FSM_System_Parallel::numOfPulses = FSM_SYSTEM_PARALLEL_NUM_OF_PULSES;
int8_t FSM_System_Parallel::pulses[FSM_SYSTEM_PARALLEL_NUM_OF_PULSES] = {
    PULSE_ESTOP_LOW,
    PULSE_ESTOP_HIGH,
    PULSE_BGR_SHORT,
    PULSE_BRS_SHORT,
    PULSE_BGS_SHORT,
    PULSE_BGS_LONG
};

FSM_System_Parallel::FSM_System_Parallel(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    currentState = FSM_System_ParallelStates::Start;
}

FSM_System_Parallel::~FSM_System_Parallel(){
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    destroyChannel(dispatcherConnectionID);
}

int8_t *FSM_System_Parallel::getPulses() {
    return pulses;
 }
int8_t FSM_System_Parallel::getNumOfPulses() {
     return numOfPulses;
}

void FSM_System_Parallel::handleMsg(){
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
        
            case PULSE_ESTOP_LOW:

                if(currentState != FSM_System_ParallelStates::EStop){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, SYSTEM_SERVICE_OUT)) {
                            perror("FSMSYSTEM: EVENT_SYSTEM_SERVICE_OUT failed\n");
                    }
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_STOP)) {
                            perror("FSMSYSTEM: MOTOR_STOP failed\n");
                    }
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, ESTOP_RECEIVED)) {
                            perror("FSMSYSTEM: ESTOP_RECEIVED failed\n");
                    }
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, SYSTEM_OPERATIONAL_OUT)) {
                                  perror("FSMSYSTEM: EVENT_SYSTEM_SERVICE_OUT failed\n");
                    }
                currentState = FSM_System_ParallelStates::EStop;
                std::cout << "FSM_SYSTEM_PARALLEL: ESTOP" << std::endl;

                }

                break;

            case PULSE_ESTOP_HIGH:
                if(currentState == FSM_System_ParallelStates::EStop){
                    currentState = FSM_System_ParallelStates::ESTOPRESOLVED;
                    std::cout << "FSM_SYSTEM_PARALLEL: ESTOPRESOLVED" << std::endl;

                }
                break;
                
            case PULSE_BGS_LONG:
                if(currentState == FSM_System_ParallelStates::Start){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, SYSTEM_SERVICE_IN)) {
                            perror("FSMSYSTEM: SYSTEM_SERVICE_IN failed\n");
                    }
                    currentState = FSM_System_ParallelStates::ServiceMode;
                    std::cout << "FSM_SYSTEM_PARALLEL: ServiceMode" << std::endl;

                }else if(currentState == FSM_System_ParallelStates::Ready){
                    calibrated = false;
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, SYSTEM_SERVICE_IN)) {
                            perror("FSMSYSTEM: SYSTEM_SERVICE_IN failed\n");
                    }
                    currentState = FSM_System_ParallelStates::Ready;
                    std::cout << "FSM_SYSTEM_PARALLEL: Ready" << std::endl;

                    }

                break;
            case PULSE_BGS_SHORT:
                if(currentState == FSM_System_ParallelStates::Ready){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, SYSTEM_OPERATIONAL_IN)) {
                            perror("FSMSYSTEM: SYSTEM_OPERATIONAL_IN failed\n");
                    }
                    currentState = FSM_System_ParallelStates::Operational;
                    std::cout << "FSM_SYSTEM_PARALLEL: Operational" << std::endl;

                }
                break;
            case PULSE_BRS_SHORT:
                if(currentState == FSM_System_ParallelStates::ServiceMode){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, SYSTEM_SERVICE_OUT)) {
                            perror("FSMSYSTEM: SYSTEM_SERVICE_OUT failed\n");
                    }
                    calibrated = true;
                    currentState = FSM_System_ParallelStates::Ready;
                    std::cout << "FSM_SYSTEM_PARALLEL: Ready" << std::endl;

                }else if(currentState == FSM_System_ParallelStates::Operational){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, SYSTEM_OPERATIONAL_OUT)) {
                            perror("FSMSYSTEM: SYSTEM_OPERATIONAL_OUT failed\n");
                    }
                    currentState = FSM_System_ParallelStates::Ready;
                    std::cout << "FSM_SYSTEM_PARALLEL: Ready" << std::endl;

                }
                break;
            case PULSE_BGR_SHORT:
                if(currentState == FSM_System_ParallelStates::ESTOPRESOLVED){
                    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, MOTOR_STOP_RESET)) {
                            perror("FSMSYSTEM: MOTOR_STOP_RESET failed\n");
                        }
                        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_FSM_PARALLEL, ESTOP_CLEARED)) {
                            perror("FSMSYSTEM: ESTOP_CLEARED failed\n");
                        }
                    if(calibrated){
                        currentState = FSM_System_ParallelStates::Ready;
                        std::cout << "FSM_SYSTEM_PARALLEL: Ready" << std::endl;

                    }else{
                        currentState = FSM_System_ParallelStates::Start;
                        std::cout << "FSM_SYSTEM_PARALLEL: Start" << std::endl;

                    }
                }

            }
        
        }
    }
}

void FSM_System_Parallel::sendMsg(){}

int32_t FSM_System_Parallel::getChannel(){
	return channelID;
}

