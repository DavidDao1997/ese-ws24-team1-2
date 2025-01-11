/*
 * ActuatorController.cpp
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#include "headers/ActuatorController.h"
#include "../Util/headers/Util.h"
#include <atomic>
#include <chrono>

std::atomic<bool> ActuatorController::lgblinking{false};
std::atomic<bool> ActuatorController::lrblinking{false};
std::atomic<bool> ActuatorController::lyblinking{false};

int8_t ActuatorController::numOfPulses = ACTUATOR_CONTROLLER_NUM_OF_PULSES;
int8_t ActuatorController::pulses_FESTO1[ACTUATOR_CONTROLLER_NUM_OF_PULSES] = {
    PULSE_MOTOR1_STOP,  
	PULSE_MOTOR1_SLOW, 
	PULSE_MOTOR1_FAST, 
	PULSE_LR1_ON,
    PULSE_LR1_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LR1_OFF,          
	PULSE_LY1_ON,
    PULSE_LY1_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LY1_OFF,       
	PULSE_LG1_ON,
    PULSE_LG1_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LG1_OFF,      
	PULSE_Q11_ON,      
	PULSE_Q11_OFF,     
    PULSE_Q21_ON,      
	PULSE_Q21_OFF,     
    PULSE_SM1_ACTIVE,   
	PULSE_SM1_RESTING, 
};

int8_t ActuatorController::pulses_FESTO2[ACTUATOR_CONTROLLER_NUM_OF_PULSES] = {
	PULSE_MOTOR2_STOP, 
	PULSE_MOTOR2_SLOW,
    PULSE_MOTOR2_FAST,  
	PULSE_LR2_ON,
    PULSE_LR2_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LR2_OFF,      
	PULSE_LY2_ON,
    PULSE_LY2_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LY2_OFF,      
	PULSE_LG2_ON,
    PULSE_LG2_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LG2_OFF,      
	PULSE_Q12_ON,      
	PULSE_Q12_OFF,
	PULSE_Q22_ON,      
	PULSE_Q22_OFF,
	PULSE_SM2_ACTIVE,  
	PULSE_SM2_RESTING,
};

// bool ActuatorController::lgblinking = false; // TODO make thread save
// bool ActuatorController::lrblinking = false;
// bool ActuatorController::lyblinking = false;

ActuatorController::ActuatorController(uint8_t festo, const std::string name, I_Actuators_Wrapper *actuatorsWrapper) {
    actuatorControllerChannel = createNamedChannel(name);
    channelID = actuatorControllerChannel->chid;
    actuators = actuatorsWrapper;
    running = false;
    greenBlinkThread = nullptr;
    yellowBlinkThread = nullptr;
    redBlinkThread = nullptr;
    festoID = festo;
    
};

ActuatorController::~ActuatorController() {
    // thread löschen
    Logger::getInstance().log(LogLevel::INFO, "destroying own channel...", "ActuatorController");
    destroyNamedChannel(channelID, actuatorControllerChannel); 
};


void ActuatorController::subscribeToDispatcher(){
    Logger::getInstance().log(LogLevel::TRACE, "Connection to Dispatcher...", "ActuatorController");
    int32_t dispatcherChannelID = 0;
    if (0 > (dispatcherChannelID = name_open(DISPATCHERNAME, NAME_FLAG_ATTACH_GLOBAL))){
        Logger::getInstance().log(LogLevel::ERROR, "Connection to Dispatcher failed...", "ActuatorController");
    }
    if (0 > MsgSendPulse(dispatcherChannelID, -1, PULSE_SUBSCRIBE, festoID)) {
        Logger::getInstance().log(LogLevel::ERROR, "Subscribing to Dispatcher failed...", "ActuatorController");
    }
    if (0 > name_close(dispatcherChannelID)) {
        Logger::getInstance().log(LogLevel::ERROR, "Closing Channel to Dispatcher failed...", "ActuatorController");
    }
}


bool ActuatorController::stop(){
    ActuatorController::lgblinking = false;
    ActuatorController::lyblinking = false;
    ActuatorController::lrblinking = false;
    if (greenBlinkThread != nullptr && greenBlinkThread->joinable()) greenBlinkThread->join();
    if (yellowBlinkThread != nullptr && yellowBlinkThread->joinable()) yellowBlinkThread->join();
    if (redBlinkThread != nullptr && redBlinkThread->joinable()) redBlinkThread->join();
	int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
            Logger::getInstance().log(LogLevel::ERROR, "shutting down MsgReceiver failed...", "ActuatorController");
            return false;
    }
    Logger::getInstance().log(LogLevel::TRACE, "Shutting down PULSE send...", "ActuatorController");
    if (0 > ConnectDetach(coid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "ActuatorController");
        return false;
    }
    return true;
}

// int8_t *ActuatorController::getPulses() { return pulses; };
// int8_t ActuatorController::getNumOfPulses() { return numOfPulses; };

void ActuatorController::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges
    _pulse msg;
    running = true;
    // lgblinking = false;
    // lrblinking = false;
    // lyblinking = false;
    while (running) {
        int recvid = MsgReceive(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "ActuatorController");
            //exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received^
            int32_t msgVal = msg.value.sival_int;
            // Logger::getInstance().log(LogLevel::TRACE, "MSG VAL: " + std::to_string(msgVal), "ActuatorController");
            switch (msg.code) {
            case PULSE_STOP_RECV_THREAD:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": received PULSE_STOP_RECV_THREAD...", "ActuatorController");
                running = false;
                break;
            case PULSE_MOTOR1_STOP:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Motor Festo 1 will be stopped...", "ActuatorController");
                actuators->motorStop();
                break;
            case PULSE_MOTOR1_SLOW:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Motor Festo 1 will be slow running...", "ActuatorController");
                actuators->runSlow();
                actuators->runRight(); // TODO test is toggle button is not needed
                break;
            case PULSE_MOTOR1_FAST:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Motor Festo 1 will be fast running...", "ActuatorController");
                actuators->runFast();
                actuators->runRight();
                break;
            case PULSE_MOTOR2_STOP:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Motor Festo 2 will be stopped...", "ActuatorController");
                actuators->motorStop();
                break;
            case PULSE_MOTOR2_SLOW:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Motor Festo 2 will be slow running...", "ActuatorController");
                actuators->runSlow();
                actuators->runRight(); // TODO test is toggle button is not needed
                break;
            case PULSE_MOTOR2_FAST:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Motor Festo 2 will be fast running...", "ActuatorController");
                actuators->runFast();
                actuators->runRight();
                break;
            case PULSE_LG1_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Green off...", "ActuatorController");
                lgblinking = false;
                actuators->greenLampLightOff();
                break;
            case PULSE_LY1_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Yellow off...", "ActuatorController");
                lyblinking = false;
                actuators->yellowLampLightOff();
                break;
            case PULSE_LR1_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Red off...", "ActuatorController");
                lrblinking = false;
                actuators->redLampLightOff();
                break;
            case PULSE_LG2_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Green off...", "ActuatorController");
                actuators->greenLampLightOff();
                break;
            case PULSE_LY2_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Yellow off...", "ActuatorController");
                lyblinking = false;
                actuators->yellowLampLightOff();
                break;
            case PULSE_LR2_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Red off...", "ActuatorController");
                lrblinking = false;
                actuators->redLampLightOff();
                break;
            case PULSE_LG1_BLINKING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Green blinking...", "ActuatorController");
                lgblinking = true;
                startGreenLampBlinkingThread(msgVal);
                break;
            case PULSE_LY1_BLINKING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Yellow blinking......", "ActuatorController");
                lyblinking = true;
                startYellowLampBlinkingThread(msgVal);
                break;
            case PULSE_LR1_BLINKING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Red blinking......", "ActuatorController");
                lrblinking = true;
                startRedLampBlinkingThread(msgVal);
                break;
            case PULSE_LG2_BLINKING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Green blinking...", "ActuatorController");
                lgblinking = true;
                startGreenLampBlinkingThread(msgVal);
                break;
            case PULSE_LY2_BLINKING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Yellow blinking......", "ActuatorController");
                lyblinking = true;
                startYellowLampBlinkingThread(msgVal);
                break;
            case PULSE_LR2_BLINKING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Red blinking......", "ActuatorController");
                lrblinking = true;
                startRedLampBlinkingThread(msgVal);
                break;
            case PULSE_LG1_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Green on......", "ActuatorController");
                lgblinking = false;
                actuators->greenLampLightOn();
                break;
            case PULSE_LY1_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Yellow on......", "ActuatorController");
                lyblinking = false;
                actuators->yellowLampLightOn();
                break;
            case PULSE_LR1_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Red on......", "ActuatorController");
                lrblinking = false;
                actuators->redLampLightOn();
                break;
            case PULSE_LG2_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Green on......", "ActuatorController");
                lgblinking = false;
                actuators->greenLampLightOn();
                break;
            case PULSE_LY2_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Yellow on......", "ActuatorController");
                lyblinking = false;
                actuators->yellowLampLightOn();
                break;
            case PULSE_LR2_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Red on......", "ActuatorController");
                lrblinking = false;
                actuators->redLampLightOn();
                break;
            case PULSE_Q11_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Q1 on......", "ActuatorController");
                actuators->Q1LightOn();
                break;
            case PULSE_Q11_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Q1 off......", "ActuatorController");
                actuators->Q1LightOff();
                break;
            case PULSE_Q12_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Q1 on......", "ActuatorController");
                actuators->Q1LightOn();
                break;
            case PULSE_Q12_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Q1 off......", "ActuatorController");
                actuators->Q1LightOff();
                break;
            case PULSE_Q21_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Q2 on......", "ActuatorController");
                actuators->Q2LightOn();
                break;
            case PULSE_Q21_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 LED Q2 off......", "ActuatorController");
                actuators->Q2LightOff();
                break;
            case PULSE_Q22_ON:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Q2 on......", "ActuatorController");
                actuators->Q2LightOn();
                break;
            case PULSE_Q22_OFF:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 LED Q2 off......", "ActuatorController");
                actuators->Q2LightOff();
                break;
			case PULSE_SM1_ACTIVE:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 SM Active......", "ActuatorController");
				actuators->closeSortingModule();
				break;
			case PULSE_SM1_RESTING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 1 SM Resting......", "ActuatorController");
				actuators->openSortingModule();
				break;
            case PULSE_SM2_ACTIVE:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 SM Active......", "ActuatorController");
				actuators->closeSortingModule();
				break;
			case PULSE_SM2_RESTING:
                Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Festo 2 SM Resting......", "ActuatorController");
				actuators->openSortingModule();
				break;
			default:
                Logger::getInstance().log(LogLevel::ERROR, std::to_string(festoID) + ": Unknown Pulse....." + std::to_string(msg.code), "ActuatorController");
            }
        } else if (msg.type == _IO_CONNECT) {
            Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoID) + ": Replying to connection...", "ActuatorController");
            MsgReply( recvid, EOK, NULL, 0 );
        }
    }
}

void ActuatorController::startRedLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    frequency = frequency >> 1;
    redBlinkThread = new std::thread([this, frequency]() { this->redlampBlinking(&lrblinking, frequency); });

    redBlinkThread->detach(); // Thread im Hintergrund ausführen lassen
}

void ActuatorController::startGreenLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    frequency = frequency >> 1;
    greenBlinkThread = new std::thread([this, frequency]() { this->greenlampBlinking(&lgblinking, frequency); });

    greenBlinkThread->detach(); // Thread im Hintergrund ausführen lassen
}

void ActuatorController::startYellowLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    frequency = frequency >> 1;
    yellowBlinkThread = new std::thread([this, frequency]() { this->yellowlampBlinking(&lyblinking, frequency); });

    yellowBlinkThread->detach(); // Thread im Hintergrund ausführen lassen
}

void ActuatorController::greenlampBlinking(std::atomic<bool> *blink, int32_t frequency) {
    while (*blink && (frequency > 0)) {

        if (*blink) actuators->greenLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        if (*blink) actuators->greenLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    Logger::getInstance().log(LogLevel::TRACE, "green blinking stopped......", "ActuatorController");
}

void ActuatorController::yellowlampBlinking(std::atomic<bool> *blink, int32_t frequency) {
    while (*blink && (frequency > 0)) {

        if (*blink) actuators->yellowLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        if (*blink) actuators->yellowLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    Logger::getInstance().log(LogLevel::TRACE, "yellow blinking stopped......", "ActuatorController");
}

void ActuatorController::redlampBlinking(std::atomic<bool> *blink, int32_t frequency) {
    while (*blink && (frequency > 0)) {

        if (*blink) actuators->redLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        if (*blink) actuators->redLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    Logger::getInstance().log(LogLevel::TRACE, "red blinking stopped......", "ActuatorController");
}


bool ActuatorController::getGreenBlinking(){
    return ActuatorController::lgblinking.load();
}  
bool ActuatorController::getYellowBlinking(){
    return ActuatorController::lyblinking.load();
}  
bool ActuatorController::getRedBlinking(){
    return ActuatorController::lrblinking.load();
}  


int32_t ActuatorController::getChannel() { return channelID; }

void ActuatorController::sendMsg() {

} // keep empty, not needed
