#ifndef IACTUATORS_WRAPPER_H
#define IACTUATORS_WRAPPER_H

#include "HALConfig.h"

class IActuators_Wrapper {
public:
	  IActuators_Wrapper() = default;
     ~IActuators_Wrapper() = default;

    // Initialization
    virtual bool init() = 0;

    // Lamp Control
    virtual void redLampLightOn() = 0;
    virtual void redLampLightOff() = 0;
    virtual void yellowLampLightOn() = 0;
    virtual void yellowLampLightOff() = 0;
    virtual void greenLampLightOn() = 0;
    virtual void greenLampLightOff() = 0;

    // Button Light Control
    virtual void startButtonLightOn() = 0;
    virtual void startButtonLightOff() = 0;
    virtual void resetButtonLightOn() = 0;
    virtual void resetButtonLightOff() = 0;

    // Indicator Control
    virtual void Q1LightOn() = 0;
    virtual void Q1LightOff() = 0;
    virtual void Q2LightOn() = 0;
    virtual void Q2LightOff() = 0;

    // Motor Control
    virtual void runRight() = 0;
    virtual void runLeft() = 0;
    virtual void runSlow() = 0;
    virtual void runFast() = 0;
    virtual void motorStop() = 0;

    // Sorting Module Control
    virtual uint8_t readSortingModule() = 0;
    virtual void openSortingModule() = 0;
    virtual void closeSortingModule() = 0;

private:
	// volatile entfernen
	volatile uintptr_t gpio_bank_0;
	volatile uintptr_t gpio_bank_1;
	volatile uintptr_t gpio_bank_2;

};

#endif // IACTUATORS_WRAPPER_H
