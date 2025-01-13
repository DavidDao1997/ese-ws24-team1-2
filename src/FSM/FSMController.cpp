#include "headers/FsmController.h"
#include <string>

#define FESTO1 0
#define FESTO2 1

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
    PULSE_BGS_LONG,
    PULSE_BRS_SHORT,
    PULSE_BGR_SHORT,
    PULSE_HS1_SAMPLE,
    PULSE_HS2_SAMPLE,
    PULSE_FSM,
    PULSE_HS1_SAMPLING_DONE,
    PULSE_HS2_SAMPLING_DONE,
    PULSE_MS_TRUE,
};

FSMController::FSMController(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;

    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), NAME_FLAG_ATTACH_GLOBAL);

    fsm = new FSM();
    subscribeToOutEvents();

    PositionTracker* positionTracker = new PositionTracker(fsm);

    fsm->enter();
    fsm->setFst_2_ready(true); // FIXME hacked

    fsm->setAReferenceHeight(2200);
    fsm->setBReferenceHeight(3150);
    fsm->setCReferenceHeight(2200);
    fsm->setAReferenceMinCount(4);
    fsm->setAReferenceMaxCount(35);
    fsm->setBReferenceMinCount(20);
    fsm->setBReferenceMaxCount(60);
    fsm->setCReferenceMinCount(10);
    fsm->setCReferenceMaxCount(45);
    fsm->setHeightThreshhold(150);
    fsm->setMaxSampleCount(130);
    
    fsm->setEStopCalibratedReturn(false);
    fsm->setServiceModeReturn(false);
    fsm->setReadyReturn(false);
}

FSMController::~FSMController() {
    // TODO WHY DOES DESTROY CHANNEL WONT WORK
    // std::cout << "FSMCONTROLLER: destroying own channel " << std::endl;
    // destroyChannel(channelID);   
    
}

int8_t *FSMController::getPulses(){ return pulses; }
int8_t FSMController::getNumOfPulses() { return numOfPulses; };
int32_t FSMController::getChannel() { return channelID; }

void FSMController::handleMsg() {
    _pulse msg;
    running = true;
     while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);
        if (recvid < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "FSMController");
        }
        
        if (recvid == 0) handlePulse(msg);
    }
}


bool FSMController::stop(){
	int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
        Logger::getInstance().log(LogLevel::ERROR, "shutting down Msg Receiver failed...", "FSMController");
        return false;
    }
    Logger::getInstance().log(LogLevel::TRACE, "Shutting down PULSE send...", "FSMController");
    if (0 > ConnectDetach(coid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "FSMController");
        return false;
    }
    return true;
}

void FSMController::sendMsg() {}

void FSMController::subscribeToOutEvents() {
        // PULSE_MOTOR1_STOP  
    fsm->getMOTOR_1_STOP().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_MOTOR1_STOP, 0, "PULSE_MOTOR1_STOP")
        )
    );
    // PULSE_MOTOR1_SLOW 
    fsm->getMOTOR_1_SLOW().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_MOTOR1_SLOW, 0, "PULSE_MOTOR1_SLOW")
        )
    );
    // PULSE_MOTOR1_FAST 
    fsm->getMOTOR_1_FAST().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_MOTOR1_FAST, 0, "PULSE_MOTOR1_FAST")
        )
    );
    // PULSE_MOTOR2_STOP 
    fsm->getMOTOR_2_STOP().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_MOTOR2_STOP, 0, "PULSE_MOTOR2_STOP")
        )
    );
    // PULSE_MOTOR2_SLOW
    fsm->getMOTOR_2_SLOW().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_MOTOR2_SLOW, 0, "PULSE_MOTOR2_SLOW")
        )
    );
    // PULSE_MOTOR2_FAST  
    fsm->getMOTOR_2_FAST().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_MOTOR2_FAST, 0, "PULSE_MOTOR2_FAST")
        )
    );
    // PULSE_LR1_ON
    fsm->getLR1_ON().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LR1_ON, 0, "PULSE_LR1_ON")
        )
    );
    // PULSE_LR1_BLINKING // msg.value int32_t (period [ms])
    // fsm->getLR1_BLINKING().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_LR1_BLINKING, 0, "PULSE_LR1_BLINKING")
    //     )
    // );
    // PULSE_LR1_OFF          
    fsm->getLR1_OFF().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LR1_OFF, 0, "PULSE_LR1_OFF")
        )
    );
    // PULSE_LY1_ON
    fsm->getLY1_ON().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LY1_ON, 0, "PULSE_LY1_ON")
        )
    );
    // PULSE_LY1_BLINKING // msg.value int32_t (period [ms])
    fsm->getLY1_BLINKING_1HZ().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LY1_BLINKING, 1000, "PULSE_LY1_BLINKING")
        )
    );
    // PULSE_LY1_OFF       
    fsm->getLY1_OFF().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LY1_OFF, 0, "PULSE_LY1_OFF")
        )
    );
    // PULSE_LG1_ON
    fsm->getLG1_ON().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LG1_ON, 0, "PULSE_LG1_ON")
        )
    );
    // PULSE_LG1_BLINKING // msg.value int32_t (period [ms])
    fsm->getLG1_BLINKING_1HZ().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LG1_BLINKING, 1000, "PULSE_LG1_BLINKING")
        )
    );
    // PULSE_LG1_OFF      
    fsm->getLG1_OFF().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LG1_OFF, 0, "PULSE_LG1_OFF")
        )
    );
    // PUK Height is Valid
    fsm->getFST_1_PUK_HEIGHT_IS_VALID().subscribe(
            *new sc::rx::subscription<void>(
                *new VoidObserver(dispatcherConnectionID, PULSE_FSM, FST_1_PUK_HEIGHT_VALID, "PULSE_FST_1_PUK_HEIGHT_VALID")
            )
        );
    // PUK Height is NOT Valid
    fsm->getFST_1_PUK_HEIGHT_IS_NOT_VALID().subscribe(
    		*new sc::rx::subscription<void>(
    			*new VoidObserver(dispatcherConnectionID, PULSE_FSM, FST_1_PUK_HEIGHT_NOT_VALID, "PULSE_FST_1_PUK_HEIGHT_NOT_VALID")
		   )
	   );
    // PULSE_LR2_ON
    fsm->getLR2_ON().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LR2_ON, 0, "PULSE_LR2_ON")
        )
    );
    // // PULSE_LR2_BLINKING // msg.value int32_t (period [ms])
    // fsm->getLR2_BLINKING().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_LR2_BLINKING, 0, "PULSE_LR2_BLINKING")
    //     )
    // );
    // PULSE_LR2_OFF      
    fsm->getLR2_OFF().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LR2_OFF, 0, "PULSE_LR2_OFF")
        )
    );
    // PULSE_LY2_ON
    fsm->getLY2_ON().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LY2_ON, 0, "PULSE_LY2_ON")
        )
    );
    // PULSE_LY2_BLINKING // msg.value int32_t (period [ms])
    fsm->getLY2_BLINKING_1HZ().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LY2_BLINKING, 1000, "PULSE_LY2_BLINKING")
        )
    );
    // PULSE_LY2_OFF      
    fsm->getLY2_OFF().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LY2_OFF, 0, "PULSE_LY2_OFF")
        )
    );
    // PULSE_LG2_ON
    fsm->getLG2_ON().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LG2_ON, 0, "PULSE_LG2_ON")
        )
    );
    // PULSE_LG2_BLINKING // msg.value int32_t (period [ms])
    fsm->getLG2_BLINKING_1HZ().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LG2_BLINKING, 1000, "PULSE_LG2_BLINKING")
        )
    );
    // PULSE_LG2_OFF      
    fsm->getLG2_OFF().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_LG2_OFF, 0, "PULSE_LG2_OFF")
        )
    );
    // PULSE_Q11_ON      
     fsm->getQ11_ON().subscribe(
         *new sc::rx::subscription<void>(
             *new VoidObserver(dispatcherConnectionID, PULSE_Q11_ON, 0, "PULSE_Q11_ON")
         )
     );
    // // PULSE_Q11_OFF     
    // fsm->getQ11_OFF().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_Q11_OFF, 0, "PULSE_Q11_OFF")
    //     )
    // );
     // PULSE_Q12_ON
     fsm->getQ12_ON().subscribe(
         *new sc::rx::subscription<void>(
             *new VoidObserver(dispatcherConnectionID, PULSE_Q12_ON, 0, "PULSE_Q12_ON")
         )
     );
    // // PULSE_Q12_OFF
    // fsm->getQ12_OFF().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_Q12_OFF, 0, "PULSE_Q12_OFF")
    //     )
    // );
    // // PULSE_Q21_ON      
    // fsm->getQ21_ON().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_Q21_ON, 0, "PULSE_Q21_ON")
    //     )
    // );
    // // PULSE_Q21_OFF     
    // fsm->getQ21_OFF().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_Q21_OFF, 0, "PULSE_Q21_OFF")
    //     )
    // );
    // // PULSE_Q22_ON      
    // fsm->getQ22_ON().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_Q22_ON, 0, "PULSE_Q22_ON")
    //     )
    // );
    // // PULSE_Q22_OFF
    // fsm->getQ22_OFF().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_Q22_OFF, 0, "PULSE_Q22_OFF")
    //     )
    // );
    // PULSE_SM1_ACTIVE   
    fsm->getFST_1_SORTING_MODULE_ACTIVE().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_SM1_ACTIVE, 0, "PULSE_SM1_ACTIVE")
        )
    );
    // PULSE_SM1_RESTING 
    fsm->getFST_1_SORTING_MODULE_RESTING().subscribe(
        *new sc::rx::subscription<void>(
            *new VoidObserver(dispatcherConnectionID, PULSE_SM1_RESTING, 0, "PULSE_SM1_RESTING")
        )
    );
    // // PULSE_SM2_ACTIVE  
    // fsm->getSM2_ACTIVE().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_SM2_ACTIVE, 0, "PULSE_SM2_ACTIVE")
    //     )
    // );
    // // PULSE_SM2_RESTING
    // fsm->getSM2_RESTING().subscribe(
    //     *new sc::rx::subscription<void>(
    //         *new VoidObserver(dispatcherConnectionID, PULSE_SM2_RESTING, 0, "PULSE_SM2_RESTING")
    //     )
    // );
}

void FSMController::handlePulse(_pulse msg) {
    int32_t msgVal = msg.value.sival_int;
    switch (msg.code) {
        case PULSE_ESTOP_HIGH:
            (msgVal == 0)?fsm->raiseESTOP_1_HIGH():fsm->raiseESTOP_2_HIGH();
            // fsm->raiseESTOP_1_HIGH();
            // fsm->raiseESTOP_2_HIGH();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_ESTOP_HIGH..." + std::to_string(msgVal), "FSMController");
            break;
        case PULSE_ESTOP_LOW:
            (msgVal == 0)?fsm->raiseESTOP_1_LOW():fsm->raiseESTOP_2_LOW();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_ESTOP_LOW..."+ std::to_string(msgVal), "FSMController");
            break;
        case PULSE_STOP_RECV_THREAD:
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_STOP_RECV_THREAD...", "FSMController");
            running = false;
            subThreadsRunning = false;             
            break;
        case PULSE_LBF_INTERRUPTED:
            (msgVal == 0)?fsm->raiseLBF_1_INTERRUPTED():fsm->raiseLBF_2_INTERRUPTED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBF_INTERRUPTED LBF_1..."+ std::to_string(msgVal), "FSMController");
            break;              
        case PULSE_LBF_OPEN:
            (msgVal == 0)?fsm->raiseLBF_1_OPEN():fsm->raiseLBF_2_OPEN();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBF_OPEN..."+ std::to_string(msgVal), "FSMController");
            // FIXME hacked
            if (msgVal == 0) {
                // fsm->raiseFST_1_POSITION_INGRESS_DISTANCE_VALID();
                // fsm->raiseFST_1_POSITION_HEIGHTMEASUREMENT_PUK_EXPECTED();
            } else {
                fsm->raiseFST_2_POSITION_INGRESS_DISTANCE_VALID();
                fsm->raiseFST_2_POSITION_HEIGHTMEASUREMENT_PUK_EXPECTED();
            }
            break;
        case PULSE_LBE_INTERRUPTED:
            (msgVal == 0)?fsm->raiseLBE_1_INTERRUPTED():fsm->raiseLBE_2_INTERRUPTED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBE_INTERRUPTED..."+ std::to_string(msgVal), "FSMController");
            break;
        case PULSE_LBE_OPEN:
            (msgVal == 0)?fsm->raiseLBE_1_OPEN():fsm->raiseLBE_2_OPEN();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBE_OPEN..."+ std::to_string(msgVal), "FSMController");
            // FIXME hacked
            if (msgVal == 0) {
                fsm->raiseFST_2_POSITION_INGRESS_PUK_EXPECTED();
            } else {
            }
            break;
        case PULSE_LBR_INTERRUPTED:
            (msgVal == 0)?fsm->raiseLBR_1_INTERRUPTED():fsm->raiseLBR_2_INTERRUPTED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBR_INTERRUPTED..."+ std::to_string(msgVal), "FSMController");
            break;
        case PULSE_LBR_OPEN:
            (msgVal == 0)?fsm->raiseLBR_1_OPEN():fsm->raiseLBR_2_OPEN();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBR_OPEN..."+ std::to_string(msgVal), "FSMController");
            break;
        case PULSE_LBM_INTERRUPTED:
            (msgVal == 0)?fsm->raiseLBM_1_INTERRUPTED():fsm->raiseLBM_2_INTERRUPTED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBM_INTERRUPTED..."+ std::to_string(msgVal), "FSMController");
            // FIXME hacked
            if (msgVal == 0) {
                fsm->raiseFST_1_PUK_SORTING_PASSTHROUGH();
                fsm->raiseFST_1_POSITION_EGRESS_PUK_EXPECTED();
            } else {
                // fsm->raiseFST_2_PUK_SORTING_PASSTHROUGH();
                // fsm->raiseFST_2_POSITION_EGRESS_PUK_EXPECTED();
            }
            break;
        case PULSE_LBM_OPEN:
            (msgVal == 0)?fsm->raiseLBM_1_OPEN():fsm->raiseLBM_2_OPEN();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBM_OPEN..."+ std::to_string(msgVal), "FSMController");
            // fsm->raiseFST_1_POSITION_EGRESS_PUK_EXPECTED();
            break;
        case PULSE_BGS_SHORT:
            (msgVal == 0)?fsm->raiseBGS_1_INTERRUPTED():fsm->raiseBGS_2_INTERRUPTED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_BGS_SHORT..."+ std::to_string(msgVal), "FSMController");
            break;
        case PULSE_BGS_LONG:
            (msgVal == 0)?fsm->raiseBGS_1_LONG_PRESSED():fsm->raiseBGS_2_LONG_PRESSED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_BGS_LONG..."+ std::to_string(msgVal), "FSMController");
            break;  
        case PULSE_BRS_SHORT:
            (msgVal == 0)?fsm->raiseBRS_1_INTERRUPTED():fsm->raiseBRS_2_INTERRUPTED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_BRS_SHORT..."+ std::to_string(msgVal), "FSMController");
            break;
        case PULSE_BGR_SHORT:
            (msgVal == 0)?fsm->raiseBGR_1_INTERRUPTED():fsm->raiseBGR_2_INTERRUPTED();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_BGR_SHORT..."+ std::to_string(msgVal), "FSMController");
            break;
        case PULSE_HS1_SAMPLE:
            fsm->setFST_1_currentValue(msgVal);
            fsm-> raiseHS_1_SAMPLE();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_HS1_SAMPLE...", "FSMController");
            break;
        case PULSE_HS2_SAMPLE:
            fsm->raiseHS_2_SAMPLE();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_HS2_SAMPLE...", "FSMController");
            break;
        case PULSE_HS1_SAMPLING_DONE:
            fsm-> raiseHS_1_SAMPLING_DONE();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_HS1_SAMPLING_DONE...", "FSMController");
            //FIXME hacked
            fsm->raiseFST_1_POSITION_SORTING_PUK_EXPECTED();
            break; 
        case PULSE_HS2_SAMPLING_DONE:
            fsm-> raiseHS_2_SAMPLING_DONE();
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_HS2_INTERRUPTED...", "FSMController");
            break;
        case PULSE_MS_TRUE:
            // (msgVal == 0)?fsm->raiseMS_1_HIGH():fsm->raiseMS_2_HIGH();
            if (msgVal == 0) {
                fsm->raiseMS_1_HIGH();
            }
            Logger::getInstance().log(LogLevel::TRACE, "received PULSE_MS_TRUE..."+ std::to_string(msgVal), "FSMController");
            break;
        // case PULSE_MS_FALSE:
        //     fsm->raiseMS_1_FALSE();    
        //     Logger::getInstance().log(LogLevel::TRACE, "received PULSE_MS_FALSE...", "FSMController");
        //     break;   
        default:
            // TODO handle non application messages, for reference see ActuatorController 
            Logger::getInstance().log(LogLevel::ERROR, "Unknown Pulse....." + std::to_string(msg.code), "FSMController");
            break;
    }
}