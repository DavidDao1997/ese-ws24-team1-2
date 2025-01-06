#include "FSMController.h"

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
    PULSE_MS_FALSE
};

class VoidObserver : public sc::rx::Observer<void> {
public:
    // Constructor that accepts a callback of type void() (a function with no parameters and no return value)
    VoidObserver(
        int coid,
        int priority,
        int code,
        int value
    ): coid(coid), priority(priority), code(code), value(value) {}

    // Override the next() method to invoke the callback
    virtual void next() override {
         if (0 < MsgSendPulse(coid, priority, code, value)) {
            Logger::getInstance().log(LogLevel::ERROR, "LG1 On Failed...", "FSMController");
        }
    }

private:
    int coid;
    int priority;
    int code;
    int value;
};

FSMController::FSMController(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;

    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);

    fsm = new FSM_QualityGate();
    VoidObserver* bigSwitchObserver = new VoidObserver(1,2,3,4);
}

//         // Similarly, you can subscribe to other Observables:
//         fsm.getMOTOR_SLOW().subscribe([this](){
//             std::cout << "FSM says: Motor SLOW" << std::endl;
//             motorController.setMotorSpeed(50); // Just an example speed
//         });

//         fsm.getMOTOR_FAST().subscribe([this](){
//             std::cout << "FSM says: Motor FAST" << std::endl;
//             motorController.setMotorSpeed(100);
//         });

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
            //exit(EXIT_FAILURE);
        }
        
        if (recvid == 0) { // Pulse received
            int32_t msgVal = msg.value.sival_int;
            switch (msg.code) {
                case PULSE_ESTOP_HIGH:
                    fsm->raiseESTOP_1_HIGH();
                    Logger::getInstance().log(LogLevel::DEBUG, "E-Stop 1 High", "FSMController");
                    break;
                case PULSE_ESTOP_LOW:
                    fsm->raiseESTOP_1_LOW();
                    Logger::getInstance().log(LogLevel::DEBUG, "E-Stop 1 Low", "FSMController");
                    break;
                case PULSE_LBF_INTERRUPTED:
                    fsm->raiseLBF_1_INTERRUPTED();
                    Logger::getInstance().log(LogLevel::DEBUG, "LBF_1 Interrupted", "FSMController");
                    break;              
                case PULSE_LBF_OPEN:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBF_OPEN...", "FSMController");
                    fsm->raiseLBF_1_OPEN();
                    break;
                case PULSE_LBE_INTERRUPTED:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    fsm->raiseLBE_1_INTERRUPTED();
                    break;
                case PULSE_LBE_OPEN:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_OPEN...", "FSMController");
                    fsm->raiseLBE_1_OPEN();
                    break;
                case PULSE_LBR_INTERRUPTED:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBR_INTERRUPTED...", "FSMController");
                    fsm->raiseLBR_1_INTERRUPTED();
                    break;
                case PULSE_LBR_OPEN:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBR_OPEN...", "FSMController");
                    fsm->raiseLBR_1_OPEN();
                    break;
                case PULSE_LBM_INTERRUPTED:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBM_INTERRUPTED...", "FSMController");
                    fsm->raiseLBM_1_INTERRUPTED();
                    break;
                case PULSE_LBM_OPEN:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBM_OPEN...", "FSMController");
                    fsm->raiseLBM_1_OPEN();
                    break;
                case PULSE_BGS_SHORT:
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_BGS_SHORT...", "FSMController");
                    fsm->raiseBGS_1_INTERRUPTED();
                    break;
                case PULSE_BGS_LONG:
                    fsm->raiseBGS_1_LONG_PRESSED();
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    break;  
                case PULSE_BRS_SHORT:
                    fsm->raiseBRS_1_INTERRUPTED();
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    break;
                case PULSE_BGR_SHORT:
                    fsm->raiseBGR_1_INTERRUPTED();
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    break;
                case PULSE_HS1_SAMPLE:
                    fsm-> raiseHS_1_SAMPLE();
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    break;
                case PULSE_HS2_SAMPLE:
                    
                    break;
                case PULSE_HS1_SAMPLING_DONE:
                    fsm-> raiseHS_1_SAMPLING_DONE();
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    break; 
                //  case PULSE_HS2_SAMPLING_DONE:
                //     fsm-> raiseHS_2_SAMPLING_DONE();
                //     Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                //     break;  
                case PULSE_MS_TRUE:
                    fsm->raiseMS_TRUE();
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    break;
                case PULSE_MS_FALSE:
                    fsm->raiseMS_FALSE();    
                    Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_LBE_INTERRUPTED...", "FSMController");
                    break;   
                default:
                    break;

            }


        }
    }
}


bool FSMController::stop(){
	int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
        Logger::getInstance().log(LogLevel::ERROR, "shutting down Msg Receiver failed...", "FSMController");
        return false;
    }
    Logger::getInstance().log(LogLevel::DEBUG, "Shutting down PULSE send...", "FSMController");
    if (0 > ConnectDetach(coid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "FSMController");
        return false;
    }
    return true;
}
void FSMController::sendMsg() {}