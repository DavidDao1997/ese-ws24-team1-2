/*
 * Decoder.cpp
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#include "headers/Decoder.h"
#include "../HAL/headers/HALConfig.h"

Decoder::Decoder(const std::string dispatcherChannelName) {
    running = false;

    channelID = createChannel();

    sensorISR = new SensorISR();
    // sonsorISR = foo;
    if (!sensorISR->registerInterrupt(channelID)){
        perror("decoder could not register interrupt")
;    }
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);

    // TODO check if festo 1 or festo 2 in parameter list
}

Decoder::~Decoder() {
    running = false;
    // TODO How to end thread if blocked in MsgReveivePulse
    destroyChannel(channelID);
}

void Decoder::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!"); // TODO does this happen on decoonstruct???
            running = false;
            return; // TODO
        }

        if (recvid == 0) { // Pulse received
            if (msg.code != PULSE_INTR_ON_PORT0) {
                perror("Unexpected Pulse!");
            }
            sensorISR->clearCurrentInterrupt();
            decode();
            std::cout << "Interrupt received entpacken und weitergabe an dispatcher" << std::endl;
        }
    }
}

void Decoder::decode() {
    uint32_t flippedValues = sensorISR->getFlippedValues();
    uint32_t previousValues = sensorISR->getPreviousValues();

    // ESTOP
    if ((flippedValues & (uint32_t)BIT_MASK(SES_PIN)) != 0) {
        // SES_PIN
        int8_t current_level = (previousValues >> SES_PIN) & 0x1;
        int32_t code = current_level ? PULSE_ESTOP_HIGH : PULSE_ESTOP_LOW;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Front
    if ((flippedValues & (uint32_t)BIT_MASK(LBF_PIN)) != 0) {
        // LBF_PIN
        uint8_t current_level = (previousValues >> LBF_PIN) & 0x1;
        // TODO Add 2 Festo support eg add festo# to value instead of sending just 0 and remove number ->
        // PULSE_LBF_INTERRUPTED
        int32_t code = current_level ? PULSE_LBF_INTERRUPTED : PULSE_LBF_OPEN;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Berrier End
    if ((flippedValues & (uint32_t)BIT_MASK(LBE_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (previousValues >> LBE_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBE_INTERRUPTED : PULSE_LBE_OPEN;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Ramp
    if ((flippedValues & (uint32_t)BIT_MASK(LBR_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (previousValues >> LBR_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBR_INTERRUPTED : PULSE_LBR_OPEN;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Metal Sensor
    if ((flippedValues & (uint32_t)BIT_MASK(LBM_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (previousValues >> LBM_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBM_INTERRUPTED : PULSE_LBM_OPEN;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Button Start
    if ((flippedValues & (uint32_t)BIT_MASK(BGS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (previousValues >> BGS_PIN) & 0x1;
        int32_t code = current_level ? PULSE_BGS_HIGH : PULSE_BGS_LOW;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Button Stop
    if ((flippedValues & (uint32_t)BIT_MASK(BRS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (previousValues >> BRS_PIN) & 0x1;
        int32_t code = current_level ? PULSE_BRS_HIGH : PULSE_BRS_LOW;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Button Reset
    if ((flippedValues & (uint32_t)BIT_MASK(BGR_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (previousValues >> BGR_PIN) & 0x1;
        int32_t code = current_level ? PULSE_BGR_HIGH : PULSE_BGR_LOW;
        sendMsg(code, 0); // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    
    // TODO if neccessary
    // if (...) {
    //     ...
    // }
    // ...
}

void Decoder::sendMsg(int8_t msgCode, int32_t msgValue) {
    // senden an den dispatcher
    if (0 > MsgSendPulse(dispatcherConnectionID, -1, msgCode, msgValue)) {
        perror("Decoder Send failed");
    }
}

int32_t Decoder::getChannel() { return channelID; }
