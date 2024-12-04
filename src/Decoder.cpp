/*
 * Decoder.cpp
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */


#include "festoheader/Decoder.h"
#include "HAL/halheader/HALConfig.h"
#include "HAL/halheader/SensorISR.h"

Decoder::Decoder(const char* name) {
    decoderChannel = createNamedChannel(name);
    channelID = decoderChannel->chid;

    // init SensorISR
}

Decoder::~Decoder(){
    // Send stop pulse to terminate any dispatch thread
    int connectionID = connectToChannel(channelID);
    if (connectionID >= 0) {
        MsgSendPulse(connectionID, -1, PULSE_STOP_THREAD, 0);
        ConnectDetach(connectionID);
    }
    destroyNamedChannel(channelID, decoderChannel);
}


void Decoder::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    bool running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received
            if (msg.code == PULSE_STOP_THREAD) {
                running = false;
            }

            if (msg.code == PULSE_INTR_ON_PORT0) {
            	handleInterrupt(); // unmask and send to dispatcher
                std::cout << "Interrupt received entpacken und weitergabe an dispatcher" << std::endl;
            }
        }
    }
}


void Decoder::handleInterrupt(void) {

    // TODO ÜBERARBEITEN DEMASKIEREN UND AN DISPATCHER

    uintptr_t gpioBase = mmap_device_io(GPIO_REGISTER_LENGHT, GPIO_PORT0);
    // uintptr_t gpioBase = SensorISR->getAddr(); // SensorISR::portBaseAddr; //?????
    int interruptID = SensorISR::interruptID;       // ????

    unsigned int intrStatusReg = in32(uintptr_t(gpioBase + GPIO_IRQSTATUS_1));
    out32(uintptr_t(gpioBase + GPIO_IRQSTATUS_1), 0xffffffff);     // Clear all interrupts.
    InterruptUnmask(INTR_GPIO_PORT0, interruptID);                 // Unmask interrupt.

    for (int pin = 0; pin < 32; pin++) {
        unsigned int mask = (uint32_t) BIT_MASK(pin);
        if ((intrStatusReg & mask) != 0) {  // Check if interrupt occurred on this pin.
            int current_level = (in32((uintptr_t) gpioBase + GPIO_DATAIN) >> pin) & 0x1;
            printf("Interrupt on pin %d, now %d\n", pin, current_level);
        }
    }
}

void Decoder::sendMsg(){
    // senden an den dispatcher

}




int32_t Decoder::getChannel(){
    return channelID;
}

