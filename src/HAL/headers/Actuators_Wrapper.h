
#ifndef ACTUATORS_WRAPPER_H
#define ACTUATORS_WRAPPER_H


#include "../interfaces/I_Actuators_Wrapper.h"


class Actuators_Wrapper : public I_Actuators_Wrapper {
public:
	
	Actuators_Wrapper();
	~Actuators_Wrapper();
	bool init() override;

	// Actors
	void redLampLightOn() override;
	void redLampLightOff() override;
	void yellowLampLightOn() override;
	void yellowLampLightOff() override;
	void greenLampLightOn() override;
	void greenLampLightOff() override;

	void startButtonLightOn() override;
	void startButtonLightOff() override;
	void resetButtonLightOn() override;
	void resetButtonLightOff() override;
	void Q1LightOn() override;
	void Q1LightOff() override;
	void Q2LightOn() override;
	void Q2LightOff() override;

// Motor
	void runRight() override;
	void runLeft() override;
	void runSlow() override;
	void runFast() override;
	void motorStop() override;


//Sorting Module
	uint8_t readSortingModule() override;
	void openSortingModule() override;
	void closeSortingModule() override;

	


private:
	// volatile entfernen
	volatile uintptr_t gpio_bank_0;
	volatile uintptr_t gpio_bank_1;
	volatile uintptr_t gpio_bank_2;



	
};


#endif /*ACTUATORS_WRAPPER_H*/
