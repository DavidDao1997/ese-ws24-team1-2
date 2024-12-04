/*
 * SensorISR.cpp
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */


#include "halheader/SensorISR.h"
#include "halheader/HALConfig.h"

// Static variables
int SensorISR::interruptID = -1;
uintptr_t SensorISR::portBaseAddr = 0;

SensorISR::SensorISR(uintptr_t gpioBaseAddr, int gpioPort, int pulseCode, int channelID) {



	// Change to code, no method call in Constructor, method might not exist yet!
	if (!initializeGPIOBaseAddr(gpioBaseAddr)) {
        throw std::runtime_error("Failed to initialize GPIO base address!");
    }
    if (!registerInterrupt(gpioPort, pulseCode, channelID)) {
        throw std::runtime_error("Failed to register interrupt!");
    }
}

SensorISR::~SensorISR() {
    if (interruptID >= 0) {
        InterruptDetach(interruptID);
    }
}

bool SensorISR::initializeGPIOBaseAddr(uintptr_t portAddr) {
    portBaseAddr = mmap_device_io(GPIO_REGISTER_LENGHT, portAddr);
    if (portBaseAddr == MAP_DEVICE_FAILED) {
        perror("Failed to map GPIO base address!");
        return false;
    }
    return true;
}

bool SensorISR::registerInterrupt(int gpioPort, int pulseCode, int channelID) {
    int connectionID = ConnectAttach(0, 0, channelID, _NTO_SIDE_CHANNEL, 0);
    if (connectionID < 0) {
        perror("Could not connect to channel!");
        return false;
    }

    struct sigevent intr_event;
    SIGEV_PULSE_INIT(&intr_event, connectionID, SIGEV_PULSE_PRIO_INHERIT, pulseCode, 0);

    interruptID = InterruptAttachEvent(gpioPort, &intr_event, 0);
    if (interruptID < 0) {
        perror("Interrupt was not able to be attached!");
        return false;
    }

    return true;
}

void SensorISR::initializeGPIOInterrupt(int pin) const {
    configureEdgeDetection(portBaseAddr, pin, true, true);

    // Enable Interrupts for Pin
    out32(portBaseAddr + GPIO_IRQSTATUS_SET_1, BIT_MASK(pin));
}

void SensorISR::configureEdgeDetection(uintptr_t baseAddr, int pin, bool rising, bool falling) const {
    unsigned int temp;

    if (rising) {
        temp = in32(baseAddr + GPIO_RISINGDETECT);
        temp |= BIT_MASK(pin);
        out32(baseAddr + GPIO_RISINGDETECT, temp);
    }

    if (falling) {
        temp = in32(baseAddr + GPIO_FALLINGDETECT);
        temp |= BIT_MASK(pin);
        out32(baseAddr + GPIO_FALLINGDETECT, temp);
    }
}


