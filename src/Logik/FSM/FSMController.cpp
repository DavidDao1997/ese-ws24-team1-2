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
    PULSE_BGS_SHORT,
    PULSE_BRS_SHORT,
    PULSE_BGR_SHORT,
    // PULSE_HS_SAMPLE
};


#define FESTO1 0
#define FESTO2 1


FSMController::FSMController(const std::string dispatcherChannelName){
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;
    // FSM_QualityGate qualityGateInstance;
    fsm = new FSM_QualityGate();

    // TODO connect to dispatcher
    //create a connection to Dispatcher
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
}


FSMController::~FSMController(){
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    // TODO disconnect from dispatcher ?? this should disconnect not destroy a channel
    destroyChannel(dispatcherConnectionID);
}

int8_t* FSMController::getPulses() { return pulses; };
int8_t FSMController::getNumOfPulses() { return numOfPulses; };

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
            case PULSE_BGS_SHORT:
                std::cout << "FSMCONTROLLER: received PULSE_BGS_SHORT " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseBGS_1_INTERRUPTED();
                } else {
                    // TODO
                }
                break;
            case PULSE_BRS_SHORT:
                std::cout << "FSMCONTROLLER: received PULSE_BRS_SHORT " << std::endl;
                if (FESTO1 == msgVal){
                	// TODO fsm->raiseBRS_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGR_SHORT:
                std::cout << "FSMCONTROLLER: received PULSE_BGR_SHORT " << std::endl;
                if (FESTO1 == msgVal){
                	fsm->raiseBGR_1_INTERRUPTED();
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


void FSMController::sendMsg(){
    // TODO check if needed when FSMs dont send Pulses    
    
    sc::rx::Observable<void> motorStop = fsm->getMOTOR_STOP();
    sc::rx::Observable<void> motorFast = fsm->getMOTOR_FAST();
    sc::rx::Observable<void> motorSlow = fsm->getMOTOR_SLOW();

    sc::rx::Observable<void> lg1On = fsm->getLG1_ON();
    sc::rx::Observable<void> lg1Blinking1Hz = fsm->getLG1_BLINKING_1HZ();
    sc::rx::Observable<void> lg1Off = fsm->getLG1_OFF();

    subThreadsRunning = true;
    FSMController *fsmController = new FSMController("foo");

    // void monitorObservable(sc::rx::Observable<void> observable, std::string pulseName, PulseCode pulseCode, int32_t pulseValue) {
    //     while (subThreadsRunning) {
    //         observable.next();
    //         if (0 > MsgSendPulse(dispatcherConnectionID, -1, pulseCode, pulseValue)) {
    //             perror("FSMController failed to send " + pulseName)
    //         }
    //     }
    // }

    std::thread motorStopThread(FSMController::monitorObservable, motorStop, "PULSE_MOTOR1_STOP", PULSE_MOTOR1_STOP, 0, subThreadsRunning, dispatcherConnectionID);
    std::thread motorFastThread(FSMController::monitorObservable, motorFast, "PULSE_MOTOR1_FAST", PULSE_MOTOR1_FAST, 0, subThreadsRunning, dispatcherConnectionID);
    std::thread motorSlowThread(FSMController::monitorObservable, motorSlow, "PULSE_MOTOR1_SLOW", PULSE_MOTOR1_SLOW, 0, subThreadsRunning, dispatcherConnectionID);
    std::thread lg1OnThread(FSMController::monitorObservable, lg1On, "PULSE_LG1_ON", PULSE_LG1_ON, 0, subThreadsRunning, dispatcherConnectionID);
    std::thread lg1Blinking1HzThread(FSMController::monitorObservable, lg1Blinking1Hz, "PULSE_LG1_BLINKING", PULSE_LG1_BLINKING, 1000, subThreadsRunning, dispatcherConnectionID);
    std::thread lg1OffThread(FSMController::monitorObservable, lg1Off, "PULSE_LG1_OFF", PULSE_LG1_OFF, 0, subThreadsRunning, dispatcherConnectionID);

    motorStopThread.join();
    motorFastThread.join();
    motorSlowThread.join();
    lg1OnThread.join();
    lg1Blinking1HzThread.join();
    lg1OffThread.join();
}

void FSMController::monitorObservable(
    sc::rx::Observable<void> observable, 
    std::string pulseName, 
    PulseCode pulseCode, 
    int32_t pulseValue,
    bool subThreadsRunning,
    int32_t dispatcherConnectionID
) {
    while (subThreadsRunning) {
        observable.next();
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, pulseCode, pulseValue)) {
            perror("FSMController failed to send "); // + pulseName
        }
    }
}