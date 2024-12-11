#include "FSM_Motor_Parallel.h"
int8_t motorForward;
int8_t motorSlow;
int8_t motorStop;
int32_t dispatcherConnectionID;
int8_t FSM_Motor_Parallel::numOfPulses = FSM_MOTOR_PARALLEL_NUM_OF_PULSES;
int8_t FSM_Motor_Parallel::pulses[FSM_MOTOR_PARALLEL_NUM_OF_PULSES] = {
    PULSE_FSM_PARALLEL,
    
};

FSM_Motor_Parallel::FSM_Motor_Parallel(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    motorStop = 0;
    motorSlow = 0;
    motorForward = 0;

}

FSM_Motor_Parallel::~FSM_Motor_Parallel(){
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    destroyChannel(dispatcherConnectionID);
}

int8_t *FSM_Motor_Parallel::getPulses() {
    return pulses;
 }
int8_t FSM_Motor_Parallel::getNumOfPulses() {
     return numOfPulses;
}

void FSM_Motor_Parallel::handleMsg(){
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
            evaluateMotor();
            switch (msg.code) {
            case PULSE_FSM_PARALLEL:
                switch(msgVal){
                    case MOTOR_FORWARD:
                        motorForward++;
                        break;
                    case MOTOR_FORWARD_RESET:
                    	motorForward--;
                       break;
                    case MOTOR_SLOW:
                    	motorSlow++;
                        break;
                    case MOTOR_SLOW_RESET:
                    	motorSlow--;
                        break;
                    case MOTOR_STOP:
                    	motorStop++;
                        break;
                    case MOTOR_STOP_RESET:
                    	motorStop--;
                        break;
                    case ESTOP_CLEARED:
                    	motorStop = 0;
                    	motorSlow = 0;
                    	motorForward = 0;
                    default:
                        break;
                }
            }
        }
        evaluateMotor();
    }
}

void FSM_Motor_Parallel::evaluateMotor(){
    if(motorStop > 0){
        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_STOP, 0)) {
            perror("Event onMotorStopIn Failed\n");
        }
        std::cout << "FSMMOTOR: STOP" << std::endl;
    }else if(motorSlow > 0){
       if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_SLOW, 0)) {
            perror("Event onMotorStopIn Failed\n");
        }
        std::cout << "FSMMOTOR: SLOW" << std::endl;

    }else if(motorForward > 0){
        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_FAST, 0)) {
            perror("Event onMotorStopIn Failed\n");
        }
        std::cout << "FSMMOTOR: FORWARD" << std::endl;

    }else{
        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_STOP, 0)) {
            perror("Event onMotorStopIn Failed\n");
        }
        std::cout << "FSMMOTOR: STOP" << std::endl;

    }
}


void FSM_Motor_Parallel::sendMsg(){}

int32_t FSM_Motor_Parallel::getChannel(){
	return channelID;
}
