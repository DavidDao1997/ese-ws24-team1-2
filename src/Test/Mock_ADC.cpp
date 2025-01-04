/*
 * Mock_ADC.cpp
 *
 *  Created on: 04.01.2025
 *      Author: Marc
 */

#include "headers/Mock_ADC.h"

Mock_ADC::Mock_ADC() {
    hsCoid = -1;
}


Mock_ADC::~Mock_ADC(){
    if (0 > ConnectDetach(hsCoid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "Mock_ADC");
    }
}


void Mock_ADC::mockInit(int32_t hscChannelId){
    hsCoid = connectToChannel(hscChannelId);
}


void Mock_ADC::sample(void){
    // TODO
    if (hsCoid == -1){
        Logger::getInstance().log(LogLevel::ERROR, "HSController Connection not set...", "Mock_ADC");
    } else {
        if (0 > MsgSendPulse(hsCoid, -1, PULSE_ADC_SAMPLE, 2000)) {
            Logger::getInstance().log(LogLevel::ERROR, "Failed to Send Pulse To HSController...", "Mock_ADC");
        }
    }
}

void Mock_ADC::init(void){}
void Mock_ADC::adcEnableSequence(unsigned int steps){}
void Mock_ADC::registerAdcISR(int connectionID, char msgType){}
void Mock_ADC::unregisterAdcISR(void){}
void Mock_ADC::adcDisable(void){}
void Mock_ADC::stepConfigure(unsigned int stepSel, Fifo fifo, PositiveInput positiveInpChannel){}
void Mock_ADC::handleMsg() {}
void Mock_ADC::sendMsg() {}
int32_t Mock_ADC::getChannel() {}
