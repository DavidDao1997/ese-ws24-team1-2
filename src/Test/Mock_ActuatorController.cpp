/*
 * ActuatorController.cpp
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#include "headers/Mock_ActuatorController.h"

std::atomic<bool> Mock_ActuatorController::lgblinking{false};
std::atomic<bool> Mock_ActuatorController::lrblinking{false};
std::atomic<bool> Mock_ActuatorController::lyblinking{false};


int8_t Mock_ActuatorController::numOfPulses = ACTUATOR_CONTROLLER_NUM_OF_PULSES;
int8_t Mock_ActuatorController::pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES] = {
    PULSE_MOTOR1_STOP,  
	PULSE_MOTOR1_SLOW, 
	PULSE_MOTOR1_FAST, 
	PULSE_MOTOR2_STOP, 
	PULSE_MOTOR2_SLOW,
    PULSE_MOTOR2_FAST,  
	PULSE_LR1_ON,
    PULSE_LR1_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LR1_OFF,      
	PULSE_LY1_ON,
    PULSE_LY1_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LY1_OFF,      
	PULSE_LG1_ON,
    PULSE_LG1_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LG1_OFF,      
	PULSE_LR2_ON,
    PULSE_LR2_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LR2_OFF,      
	PULSE_LY2_ON,
    PULSE_LY2_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LY2_OFF,      
	PULSE_LG2_ON,
    PULSE_LG2_BLINKING, // msg.value int32_t (period [ms])
    PULSE_LG2_OFF,      
	PULSE_Q11_ON,      
	PULSE_Q11_OFF,     
	PULSE_Q12_ON,      
	PULSE_Q12_OFF,
    PULSE_SM1_ACTIVE,   
	PULSE_SM1_RESTING, 
	PULSE_SM2_ACTIVE,  
	PULSE_SM2_RESTING,
};

// bool Mock_ActuatorController::lgblinking = false; // TODO make thread save
// bool Mock_ActuatorController::lrblinking = false;
// bool Mock_ActuatorController::lyblinking = false;

Mock_ActuatorController::Mock_ActuatorController(const std::string name, Mock_Actuators_Wrapper *actuatorsWrapper) {
    actuatorControllerChannel = createNamedChannel(name);
    channelID = actuatorControllerChannel->chid;
    actuators = actuatorsWrapper;
    running = false;
};

Mock_ActuatorController::~Mock_ActuatorController() {
    // thread löschen
    std::cout << "ACTUATORCONTROLLER: destroying own channel " << std::endl;
    destroyNamedChannel(channelID, actuatorControllerChannel); 
};

bool Mock_ActuatorController::stop(){
	int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
            perror("ACTUATORCONTROLLER: shutting down Msg Receiver failed");
            return false;
    }
    std::cout << "ACTUATORCONTROLLER: Shutting down PULSE send " << std::endl;
    if (0 > ConnectDetach(coid)){
        perror("ACTUATORCONTROLLER: Stop Detach failed");
        return false;
    }
    return true;
}

int8_t *Mock_ActuatorController::getPulses() { return pulses; };
int8_t Mock_ActuatorController::getNumOfPulses() { return numOfPulses; };

void Mock_ActuatorController::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges
    _pulse msg;
    running = true;
    // lgblinking = false;
    // lrblinking = false;
    // lyblinking = false;

 

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("ACTUATORCONTROLLER: MsgReceivePulse failed!");
            //exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received^
            int32_t msgVal = msg.value.sival_int;
            // std::cout << "MSG VAL: " << msgVal << std::endl;
            switch (msg.code) {
            case PULSE_STOP_RECV_THREAD:
                std::cout << "ACTUATORCONTROLLER: received PULSE_STOP_RECV_THREAD " << std::endl;
                running = false;
                break;
            case PULSE_MOTOR1_STOP:
                std::cout << "ACTUATORCONTROLLER: Motor will be stopped" << std::endl;
                actuators->motorStop();
                break;
            case PULSE_MOTOR1_SLOW:
                std::cout << "ACTUATORCONTROLLER: Motor will be slow running" << std::endl;
                actuators->runSlow();
                actuators->runRight(); // TODO test is toggle button is not needed
                break;
            case PULSE_MOTOR1_FAST:
                std::cout << "ACTUATORCONTROLLER: Motor will be fast running" << std::endl;
                actuators->runFast();
                actuators->runRight();
                break;
            case PULSE_MOTOR2_STOP:
                std::cout << "ACTUATORCONTROLLER: Motor will be stopped" << std::endl;
                actuators->motorStop();
                break;
            case PULSE_MOTOR2_SLOW:
                std::cout << "ACTUATORCONTROLLER: Motor will be slow running" << std::endl;
                actuators->runSlow();
                actuators->runRight(); // TODO test is toggle button is not needed
                break;
            case PULSE_MOTOR2_FAST:
                std::cout << "ACTUATORCONTROLLER: Motor will be fast running" << std::endl;
                actuators->runFast();
                actuators->runRight();
                break;
            case PULSE_LG1_OFF:
                std::cout << "ACTUATORCONTROLLER: LED GREEN OFF" << std::endl;
                // lgblinking = false;
                Mock_ActuatorController::lgblinking = false;
                actuators->greenLampLightOff();
                break;
            case PULSE_LY1_OFF:
                std::cout << "ACTUATORCONTROLLER: LED YELLOW OFF" << std::endl;
                //lyblinking = false;
                Mock_ActuatorController::lyblinking = false;
                actuators->yellowLampLightOff();
                break;
            case PULSE_LR1_OFF:
                std::cout << "ACTUATORCONTROLLER: LED RED OFF" << std::endl;
                // lrblinking = false;
                Mock_ActuatorController::lrblinking = false;
                actuators->redLampLightOff();
                break;
            case PULSE_LG1_BLINKING:
                std::cout << "ACTUATORCONTROLLER: LED GREEN BLINKING" << std::endl;
                lgblinking = true;
                Mock_ActuatorController::lgblinking = true;
                startGreenLampBlinkingThread(msgVal);
                break;
            case PULSE_LY1_BLINKING:
                std::cout << "ACTUATORCONTROLLER: LED YELLOW BLINKING" << std::endl;
                // lyblinking = true;
                Mock_ActuatorController::lyblinking = true;
                startYellowLampBlinkingThread(msgVal);
                break;
            case PULSE_LR1_BLINKING:
                std::cout << "ACTUATORCONTROLLER: LED RED BLINKING" << std::endl;
                // lrblinking = true;
                Mock_ActuatorController::lrblinking = true;
                startRedLampBlinkingThread(msgVal);
                break;
            case PULSE_LG1_ON:
                std::cout << "ACTUATORCONTROLLER: LED GREEN ON" << std::endl;
                //lgblinking = false;
                Mock_ActuatorController::lgblinking = false;
                actuators->greenLampLightOn();
                break;
            case PULSE_LY1_ON:
                std::cout << "ACTUATORCONTROLLER: LED YELLOW ON" << std::endl;
                //lyblinking = false;
                Mock_ActuatorController::lyblinking = false;
                actuators->yellowLampLightOn();
                break;
            case PULSE_LR1_ON:
                std::cout << "ACTUATORCONTROLLER: LED RED ON" << std::endl;
                //lrblinking = false;
                Mock_ActuatorController::lrblinking = false;
                actuators->redLampLightOn();
                break;
			case PULSE_SM1_ACTIVE:
				actuators->closeSortingModule();
				break;
			case PULSE_SM1_RESTING:
				actuators->openSortingModule();
				break;
			default:
				std::cout << "ACTUATORCONTROLLER: UNKNOW PULSE" << std::endl;
            }
        }
    }
}

void Mock_ActuatorController::startRedLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    std::thread redBlinkThread([this, frequency]() { this->redlampBlinking(&lrblinking, frequency); });

    redBlinkThread.detach(); // Thread im Hintergrund ausführen lassen
}

void Mock_ActuatorController::startGreenLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    std::thread greenBlinkThread([this, frequency]() { this->greenlampBlinking(&lgblinking, frequency); });

    greenBlinkThread.detach(); // Thread im Hintergrund ausführen lassen
}

void Mock_ActuatorController::startYellowLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    std::thread yellowBlinkThread([this, frequency]() { this->yellowlampBlinking(&lyblinking, frequency); });

    yellowBlinkThread.detach(); // Thread im Hintergrund ausführen lassen
}

void Mock_ActuatorController::greenlampBlinking(std::atomic<bool> *blink, int32_t frequency) {
    actuators->toggleGreenBlinking();
    while (*blink && (frequency > 0)) {

        actuators->greenLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        actuators->greenLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    actuators->toggleGreenBlinking();
    std::cout << "ACTUATORCONTROLLER: green blinking stopped" << std::endl;
}

void Mock_ActuatorController::yellowlampBlinking(std::atomic<bool> *blink, int32_t frequency) {
    actuators->toggleYellowBlinking();
    while (*blink && (frequency > 0)) {

        actuators->yellowLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        actuators->yellowLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    actuators->toggleYellowBlinking();
    std::cout << "ACTUATORCONTROLLER: yellow blinking stopped" << std::endl;
}

void Mock_ActuatorController::redlampBlinking(std::atomic<bool> *blink, int32_t frequency) {
    actuators->toggleRedBlinking();
    while (*blink && (frequency > 0)) {

        actuators->redLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        actuators->redLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    actuators->toggleRedBlinking();
    std::cout << "ACTUATORCONTROLLER: red blinking stopped" << std::endl;
}

int32_t Mock_ActuatorController::getChannel() { return channelID; }

void Mock_ActuatorController::sendMsg() {} // keep empty, not needed
