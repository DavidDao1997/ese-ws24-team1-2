
#ifndef ACTUATORS_WRAPPER_H
#define ACTUATORS_WRAPPER_H

#include "HALConfig.h"
#include "I_Actuators.h"


class Actuators_Wrapper : I_Actuators {
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

	


private:

	volatile uintptr_t gpio_bank_1;
	volatile uintptr_t gpio_bank_2;



	
};


#endif /*ACTUATORS_WRAPPER_H*/
