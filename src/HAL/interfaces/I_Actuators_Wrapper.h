/*
 * I_Actuators_Wrapper.h
 *
 *  Created on: 20.12.2024
 *      Author: Marc
 */

#ifndef HAL_INTERFACES_I_ACTUATORS_WRAPPER_H_
#define HAL_INTERFACES_I_ACTUATORS_WRAPPER_H_

#include "../headers/HALConfig.h"
#include "hw/inout.h"
#include "sys/mman.h"
#include "sys/neutrino.h"
#include <iostream>
#include <stdint.h>

class I_Actuators_Wrapper  {
public:
	
	
	virtual bool init() = 0;

	// Actors
	virtual void redLampLightOn() = 0;
	virtual void redLampLightOff() = 0;
	virtual void yellowLampLightOn() = 0;
	virtual void yellowLampLightOff() = 0;
	virtual void greenLampLightOn() = 0;
	virtual void greenLampLightOff() = 0;

	virtual void startButtonLightOn() = 0;
	virtual void startButtonLightOff() = 0;
	virtual void resetButtonLightOn() = 0;
	virtual void resetButtonLightOff() = 0;
	virtual void Q1LightOn() = 0;
	virtual void Q1LightOff() = 0;
	virtual void Q2LightOn() = 0;
	virtual void Q2LightOff() = 0;

// Motor
	virtual void runRight() = 0;
	virtual void runLeft() = 0;
	virtual void runSlow() = 0;
	virtual void runFast() = 0;
	virtual void motorStop() = 0;


//Sorting Module
	virtual uint8_t readSortingModule() = 0;
	virtual void openSortingModule() = 0;
	virtual void closeSortingModule() = 0;

	
    virtual ~I_Actuators_Wrapper() {};

private:
	
	
};



#endif /* HAL_INTERFACES_I_ACTUATORS_WRAPPER_H_ */
