/*
 * Mock_ADC.cpp
 *
 *  Created on: 04.01.2025
 *      Author: Marc
 */

#include "headers/Mock_ADC.h"

Mock_ADC::Mock_ADC(uint32_t _bandHeight, std::function<void()> _onSamplingDone) {
    hsCoid = -1;
    bandHeight = _bandHeight;
    onSamplingDone = _onSamplingDone;
}


Mock_ADC::~Mock_ADC(){
    if (0 > ConnectDetach(hsCoid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "Mock_ADC");
    }
}

void Mock_ADC::registerAdcISR(int connectionID, char msgType) {
    hsCoid = connectionID;
}

void Mock_ADC::mockInit(int32_t hscChannelId){
    hsCoid = connectToChannel(hscChannelId);
}


void Mock_ADC::sample(void){
    // TODO
    // if (sampleCnt < 10) {
    //     sampleVal = bandHeight;
    // } else if (sampleCnt < 13 ) {
    //     sampleVal = firstHeight;
    // } else if (sampleCnt < 46) {
    //     sampleVal = secondHeight;
    // } else if (sampleCnt < 69) {
    //     sampleVal = thirdHeight;
    // } else {
    //     sampleVal = bandHeight;
    // }
    std::lock_guard<std::mutex> lock(mutex);
    if (!sampling) {
        sampleVal = bandHeight;
    } else if (sampleCnt < firstHeightCnt) {
        sampleVal = firstHeight;
    } else if (sampleCnt < firstHeightCnt + secondHeightCnt) {
        sampleVal = secondHeight;
    } else if (sampleCnt < firstHeightCnt + secondHeightCnt + thirdHeightCnt) {
        sampleVal = thirdHeight;
    } else {
        sampling = false;
        onSamplingDone();
        cv.notify_all();
        sampleVal = bandHeight;
    }
   // Logger::getInstance().log(LogLevel::DEBUG, "SAmple Count is: ..." + std::to_string(sampleCnt) + "Threshhold" +  std::to_string(bandHeightCnt + firstHeightCnt + secondHeightCnt + thirdHeight), "Mock_ADC");

    if (hsCoid == -1){
        Logger::getInstance().log(LogLevel::ERROR, "HSController Connection not set...", "Mock_ADC");
    } else {
        if (0 > MsgSendPulse(hsCoid, -1, PULSE_ADC_SAMPLE, sampleVal)) {
            Logger::getInstance().log(LogLevel::ERROR, "Failed to Send Pulse To HSController...", "Mock_ADC");
        }
    }
    sampleCnt++;
}

void Mock_ADC::setSample(int32_t first, int32_t second, int32_t third, int32_t firstCnt, int32_t secondCnt, int32_t thirdCnt){
    firstHeight = first;
    secondHeight = second;
    thirdHeight = third;

    if (firstCnt < 3 || secondCnt < 3 || thirdCnt < 3) throw std::runtime_error("ïllegal arguments exception: values must be larger then 2 to allow error correction");
    firstHeightCnt = firstCnt;
    secondHeightCnt = secondCnt;
    thirdHeightCnt = thirdCnt;

    sampleCnt = 0;
    sampling = true;
}

void Mock_ADC::blockUntilSamplingDone() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this](){ return !sampling; }); 
}

void Mock_ADC::init(void){}
void Mock_ADC::adcEnableSequence(unsigned int steps){}
void Mock_ADC::unregisterAdcISR(void){}
void Mock_ADC::adcDisable(void){}
void Mock_ADC::stepConfigure(unsigned int stepSel, Fifo fifo, PositiveInput positiveInpChannel){}
void Mock_ADC::handleMsg() {}
void Mock_ADC::sendMsg() {}
int32_t Mock_ADC::getChannel() {return 0;}
