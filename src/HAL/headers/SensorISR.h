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

#include "../../Logging/headers/Logger.h"
#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "HALConfig.h"

class SensorISR {
  public:
    SensorISR();
    ~SensorISR();

    bool registerInterrupt(int32_t channelID);
    void clearCurrentInterrupt();
    uint32_t getFlippedValues();
    uint32_t getCurrentValues(); // TODO check if these are ACTUALLY the previous values

    void initializeGPIOInterrupt(int pin) const;
    //     static bool registerInterrupt(int gpioPort, int pulseCode, int channelID);
    //     void initializeGPIOInterrupt(int pin) const;
    //     static int getInterruptID() { return interruptID; }

  private:
    bool initializeGPIOBaseAddr(uintptr_t portAddr);
    uintptr_t gpioBase;
    int32_t interruptID;
    void configureEdgeDetection(uintptr_t baseAddr, int pin, bool rising, bool falling) const;
};

#endif // SENSOR_ISR_H
