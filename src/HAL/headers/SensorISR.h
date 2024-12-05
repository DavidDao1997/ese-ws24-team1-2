/*
 * SensorISR.h
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#ifndef SENSOR_ISR_H
#define SENSOR_ISR_H

#include <hw/inout.h>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/neutrino.h>

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "HALConfig.h"

class SensorISR {
  public:
    SensorISR();
    ~SensorISR();

    void registerInterrupt(int32_t channelID);
    void clearCurrentInterrupt();
    uint32_t getFlippedValues();
    uint32_t getPreviousValues(); // TODO check if these are ACTUALLY the previous values

    //     static bool initializeGPIOBaseAddr(uintptr_t portAddr);
    //     static bool registerInterrupt(int gpioPort, int pulseCode, int channelID);
    //     void initializeGPIOInterrupt(int pin) const;
    //     static int getInterruptID() { return interruptID; }

  private:
    uintptr_t gpioBase;
    //     void configureEdgeDetection(uintptr_t baseAddr, int pin, bool rising, bool falling) const;
};

#endif // SENSOR_ISR_H
