/*
 * ActuatorsWrapper.h
 *
 *  Created on: 08.12.2024
 *      Author: Jannik
 */

#ifndef ACTUATORS_WRAPPER_H_
#define ACTUATORS_WRAPPER_H_

#include "HALConfig.h"
#include "IActuators_Wrapper.h"

class Actuators_Wrapper: public IActuators_Wrapper {
	  	  Actuators_Wrapper();
	  	  ~Actuators_Wrapper();

	    // Initialization
	     bool init() = 0;

	    // Lamp Control
	     void redLampLightOn() = 0;
	     void redLampLightOff() = 0;
	     void yellowLampLightOn() = 0;
	     void yellowLampLightOff() = 0;
	     void greenLampLightOn() = 0;
	     void greenLampLightOff() = 0;

	    // Button Light Control
	     void startButtonLightOn() = 0;
	     void startButtonLightOff() = 0;
	     void resetButtonLightOn() = 0;
	     void resetButtonLightOff() = 0;

	    // Indicator Control
	     void Q1LightOn() = 0;
	     void Q1LightOff() = 0;
	     void Q2LightOn() = 0;
	     void Q2LightOff() = 0;

	    // Motor Control
	     void runRight() = 0;
	     void runLeft() = 0;
	     void runSlow() = 0;
	     void runFast() = 0;
	     void motorStop() = 0;

	    // Sorting Module Control
	     uint8_t readSortingModule() = 0;
	     void openSortingModule() = 0;
	     void closeSortingModule() = 0;

		private:
	     	 // volatile entfernen
	     	 volatile uintptr_t gpio_bank_0;
	     	 volatile uintptr_t gpio_bank_1;
	     	 volatile uintptr_t gpio_bank_2;

};





#endif /* HAL_HEADERS_ACTUATORSWRAPPER_H_ */
