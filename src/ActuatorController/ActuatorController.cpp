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

int8_t ActuatorController::numOfPulses = ACTUATOR_CONTROLLER_NUM_OF_PULSES;
int8_t ActuatorController::pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES] = {
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

bool ActuatorController::lgblinking = false; // TODO make thread save
bool ActuatorController::lrblinking = false;
bool ActuatorController::lyblinking = false;

ActuatorController::ActuatorController(const std::string name, Actuators_Wrapper *actuatorsWrapper) {
    actuatorControllerChannel = createNamedChannel(name);
    channelID = actuatorControllerChannel->chid;
    actuators = actuatorsWrapper;
    running = false;
};

ActuatorController::~ActuatorController() {
    // thread löschen
    running = false;
    // attach löschen
    destroyNamedChannel(channelID, actuatorControllerChannel);
};

int8_t *ActuatorController::getPulses() { return pulses; };
int8_t ActuatorController::getNumOfPulses() { return numOfPulses; };

void ActuatorController::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges
    _pulse msg;
    running = true;
    lgblinking = false;
    lrblinking = false;
    lyblinking = false;
    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received^
            int32_t msgVal = msg.value.sival_int;
            // std::cout << "MSG VAL: " << msgVal << std::endl;
            switch (msg.code) {
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
                lgblinking = false;
                actuators->greenLampLightOff();
                break;
            case PULSE_LY1_OFF:
                std::cout << "ACTUATORCONTROLLER: LED YELLOW OFF" << std::endl;
                lyblinking = false;
                actuators->yellowLampLightOff();
                break;
            case PULSE_LR1_OFF:
                std::cout << "ACTUATORCONTROLLER: LED RED OFF" << std::endl;
                lrblinking = false;
                actuators->redLampLightOff();
                break;
            case PULSE_LG1_BLINKING:
                std::cout << "ACTUATORCONTROLLER: LED GREEN BLINKING" << std::endl;
                lgblinking = true;
                startGreenLampBlinkingThread(msgVal);
                break;
            case PULSE_LY1_BLINKING:
                std::cout << "ACTUATORCONTROLLER: LED YELLOW BLINKING" << std::endl;
                lyblinking = true;
                startYellowLampBlinkingThread(msgVal);
                break;
            case PULSE_LR1_BLINKING:
                std::cout << "ACTUATORCONTROLLER: LED RED BLINKING" << std::endl;
                lrblinking = true;
                startRedLampBlinkingThread(msgVal);
                break;
            case PULSE_LG1_ON:
                std::cout << "ACTUATORCONTROLLER: LED GREEN ON" << std::endl;
                lgblinking = false;
                actuators->greenLampLightOn();
                break;
            case PULSE_LY1_ON:
                std::cout << "ACTUATORCONTROLLER: LED YELLOW ON" << std::endl;
                lyblinking = false;
                actuators->yellowLampLightOn();
                break;
            case PULSE_LR1_ON:
                std::cout << "ACTUATORCONTROLLER: LED RED ON" << std::endl;
                lrblinking = false;
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

void ActuatorController::startRedLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    std::thread redBlinkThread([this, frequency]() { this->redlampBlinking(&lrblinking, frequency); });

    redBlinkThread.detach(); // Thread im Hintergrund ausführen lassen
}

void ActuatorController::startGreenLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    std::thread greenBlinkThread([this, frequency]() { this->greenlampBlinking(&lgblinking, frequency); });

    greenBlinkThread.detach(); // Thread im Hintergrund ausführen lassen
}

void ActuatorController::startYellowLampBlinkingThread(int frequency) {
    // Hier wird der Thread außerhalb des Switch-Blocks gestartet
    std::thread yellowBlinkThread([this, frequency]() { this->yellowlampBlinking(&lyblinking, frequency); });

    yellowBlinkThread.detach(); // Thread im Hintergrund ausführen lassen
}

void ActuatorController::greenlampBlinking(bool *blink, int32_t frequency) {
    while (*blink && (frequency > 0)) {

        actuators->greenLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        actuators->greenLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    std::cout << "green blinking stopped" << std::endl;
}

void ActuatorController::yellowlampBlinking(bool *blink, int32_t frequency) {
    while (*blink && (frequency > 0)) {

        actuators->yellowLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        actuators->yellowLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    std::cout << "yellow blinking stopped" << std::endl;
}

void ActuatorController::redlampBlinking(bool *blink, int32_t frequency) {
    while (*blink && (frequency > 0)) {

        actuators->redLampLightOff();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        actuators->redLampLightOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
    }
    std::cout << "red blinking stopped" << std::endl;
}

int32_t ActuatorController::getChannel() { return channelID; }

void ActuatorController::sendMsg() {} // keep empty, not needed
