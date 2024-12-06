
#ifndef ACTUATORS_WRAPPER_H
#define ACTUATORS_WRAPPER_H

#include "HALConfig.h"



class Actuators_Wrapper {
public:
	
	Actuators_Wrapper();
	~Actuators_Wrapper();
	bool init();

	// Actors
	void redLampLightOn();
	void redLampLightOff();
	void yellowLampLightOn();
	void yellowLampLightOff();
	void greenLampLightOn();
	void greenLampLightOff();

	void startButtonLightOn();
	void startButtonLightOff();
	void resetButtonLightOn();
	void resetButtonLightOff();
	void Q1LightOn();
	void Q1LightOff();
	void Q2LightOn();
	void Q2LightOff();

// Motor
	void runRight();
	void runLeft();
	void runSlow();
	void runFast();
	void motorStop();


//Sorting Module
	uint8_t readSortingModule();
	void openSortingModule();
	void closeSortingModule();

	


private:
	// volatile entfernen
	volatile uintptr_t gpio_bank_0;
	volatile uintptr_t gpio_bank_1;
	volatile uintptr_t gpio_bank_2;



	
};


#endif /*ACTUATORS_WRAPPER_H*/
