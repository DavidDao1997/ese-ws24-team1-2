/*
 * SensorISR.h
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#ifndef SENSOR_ISR_H
#define SENSOR_ISR_H

#include <iostream>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <stdexcept>

class SensorISR {
public:
    static int interruptID;
    static uintptr_t portBaseAddr;

    SensorISR(uintptr_t gpioBaseAddr, int gpioPort, int pulseCode, int channelID);
    ~SensorISR();

    static bool initializeGPIOBaseAddr(uintptr_t portAddr);
    static bool registerInterrupt(int gpioPort, int pulseCode, int channelID);
    void initializeGPIOInterrupt(int pin) const;
    static int getInterruptID() { return interruptID; }

private:
    void configureEdgeDetection(uintptr_t baseAddr, int pin, bool rising, bool falling) const;
};

#endif // SENSOR_ISR_H
