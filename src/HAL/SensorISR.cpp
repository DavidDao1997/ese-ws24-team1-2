/*
 * SensorISR.cpp
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#include "headers/SensorISR.h"

// Static variables
// int SensorISR::interruptID = -1;
// uintptr_t SensorISR::portBaseAddr = 0;

SensorISR::SensorISR() {

    // TODO ÜBERARBEITEN DEMASKIEREN UND AN DISPATCHER

    if (!initializeGPIOBaseAddr(GPIO_BANK_0)) {
        throw std::runtime_error("Failed to initialize GPIO base address!");
    }

    // TODO INIT GPIO INTERRUPTS
}

SensorISR::~SensorISR() {
    if (interruptID >= 0) {
        InterruptDetach(interruptID);
    }
}

bool SensorISR::registerInterrupt(int32_t decoderChannelID) {
    int connectionID = ConnectAttach(0, 0, decoderChannelID, _NTO_SIDE_CHANNEL, 0);
    if (connectionID < 0) {
        perror("Could not connect to Decoder!");
        return false;
    }

    struct sigevent intr_event;
    SIGEV_PULSE_INIT(&intr_event, connectionID, SIGEV_PULSE_PRIO_INHERIT, PULSE_INTR_ON_PORT0, 0);

    interruptID = InterruptAttachEvent(GPIO_BANK_0, &intr_event, 0);
    if (interruptID < 0) {
        perror("Interrupt was not able to be attached!");
        return false;
    }
    return true;
}

void SensorISR::clearCurrentInterrupt() {
    out32(uintptr_t(gpioBase + GPIO_IRQSTATUS_1), 0xffffffff); // Clear all interrupts.
    InterruptUnmask(INTR_GPIO_PORT0, interruptID);             // Unmask interrupt.
}
uint32_t SensorISR::getFlippedValues() { return in32(uintptr_t(gpioBase + GPIO_IRQSTATUS_1)); }
uint32_t SensorISR::getPreviousValues() { return in32((uintptr_t)gpioBase + GPIO_DATAIN); }

bool SensorISR::initializeGPIOBaseAddr(uintptr_t portAddr) {
    gpioBase = mmap_device_io(GPIO_REGISTER_LENGHT, portAddr);
    if (gpioBase == MAP_DEVICE_FAILED) {
        perror("Failed to map GPIO base address!");
        return false;
    }
    return true;
}


// void SensorISR::initializeGPIOInterrupt(int pin) const {
//     configureEdgeDetection(portBaseAddr, pin, true, true);

//     // Enable Interrupts for Pin
//     out32(portBaseAddr + GPIO_IRQSTATUS_SET_1, BIT_MASK(pin));
// }

// void SensorISR::configureEdgeDetection(uintptr_t baseAddr, int pin, bool rising, bool falling) const {
//     unsigned int temp;

//     if (rising) {
//         temp = in32(baseAddr + GPIO_RISINGDETECT);
//         temp |= BIT_MASK(pin);
//         out32(baseAddr + GPIO_RISINGDETECT, temp);
//     }

//     if (falling) {
//         temp = in32(baseAddr + GPIO_FALLINGDETECT);
//         temp |= BIT_MASK(pin);
//         out32(baseAddr + GPIO_FALLINGDETECT, temp);
//     }
// }
