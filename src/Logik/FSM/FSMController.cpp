/*
 * FSMController.cpp
 *
 *  Created on: 07.12.2024
 *      Author: Marc
 */

#include "headers/FSMController.h"

int8_t FSMController::numOfPulses = FSM_CONTROLLER_NUM_OF_PULSES;
int8_t FSMController::pulses[FSM_CONTROLLER_NUM_OF_PULSES] = {
    PULSE_ESTOP_HIGH,
    PULSE_ESTOP_LOW,
    PULSE_LBF_INTERRUPTED,
    PULSE_LBF_OPEN,
    PULSE_LBE_INTERRUPTED,
    PULSE_LBE_OPEN,
    PULSE_LBR_INTERRUPTED,
    PULSE_LBR_OPEN,
    PULSE_LBM_INTERRUPTED,
    PULSE_LBM_OPEN,
    PULSE_BGS_HIGH,
    PULSE_BGS_LOW,
    PULSE_BRS_HIGH,
    PULSE_BRS_LOW,
    PULSE_BGR_HIGH,
    PULSE_BGR_LOW,
    // PULSE_HS_SAMPLE
};


#define FESTO1 0
#define FESTO2 1


FSMController::FSMController(int32_t dispatcherChannel){
    dispatcherChannelID = dispatcherChannel;
    channelID = createChannel();
    running = false;
    // FSM_QualityGate qualityGateInstance;
    fsm = new FSM_QualityGate();
}


FSMController::~FSMController(){
    running = false;
    destroyChannel(channelID);
}

void FSMController::handleMsg() {
    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received
            int32_t msgVal = msg.value.sival_int;
            switch (msg.code) {
            case PULSE_ESTOP_HIGH:
                std::cout << "FSMCONTROLLER: received PULSE_ESTOP_HIGH " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseESTOP_1_HIGH();
                } else {
                	// TODO
                }
                break;
            case PULSE_ESTOP_LOW:
                std::cout << "FSMCONTROLLER: received PULSE_ESTOP_LOW " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseESTOP_1_LOW();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBF_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBF_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseLBF_1_INTERRUPTED();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBF_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBF_OPEN " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseLBF_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBE_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBE_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseLBE_1_INTERRUPTED();
                } else {
                    // TODO
                }            
                break;
            case PULSE_LBE_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBE_OPEN " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseLBE_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBR_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBR_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseLBF_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBR_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBR_OPEN " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseLBR_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBM_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBM_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseLBM_1_INTERRUPTED();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBM_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBM_OPEN " << std::endl;
                if (FESTO1 == msgVal){
                	// TODO fsm->raiseLBM_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGS_HIGH:
                std::cout << "FSMCONTROLLER: received PULSE_BGS_HIGH " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseBGS_1_INTERRUPTED();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGS_LOW:
                std::cout << "FSMCONTROLLER: received PULSE_BGS_LOW " << std::endl;
                if (FESTO1 == msgVal){
                	// TODO fsm->raiseBGS_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_BRS_HIGH:
                std::cout << "FSMCONTROLLER: received PULSE_BRS_HIGH " << std::endl;
                if (FESTO1 == msgVal){
                	// TODO fsm->raiseBRS_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_BRS_LOW:
                std::cout << "FSMCONTROLLER: received PULSE_BRS_LOW " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseBRS_1_INTERRUPTED();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGR_HIGH:
                std::cout << "FSMCONTROLLER: received PULSE_BGR_HIGH " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseBGR_1_INTERRUPTED();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGR_LOW:
                std::cout << "FSMCONTROLLER: received PULSE_BGR_LOW " << std::endl;
                if (FESTO1 == msgVal){
                	// TODO fsm->raiseBGR_1_OPEN();
                } else {
                    // TODO
                }
                break;
            // TODO 
            // case PULSE_HS_SAMPLE:
            //     std::cout << "FSMCONTROLLER: received PULSE_BGR_LOW " << std::endl;
            //     
            // 
            //     break;
            default:
                std::cout << "FSMCONTROLLER: received UNKNOWN! ERROR " << std::endl;
                // TODO FEHLER 
            }
        }
    }
}


int32_t FSMController::getChannel(){
    return channelID;
}


void FSMController::sendMsg(int8_t msgCode, int32_t msgValue){
    // TODO check if needed when FSMs dont send Pulses    
}
