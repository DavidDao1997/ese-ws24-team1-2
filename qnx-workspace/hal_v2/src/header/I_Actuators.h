
#ifndef BUTTONLED_H
#define BUTTONLED_H

#include "HALConfig.h"

class I_Actuators {
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


	virtual ~I_Actuators() {};
};


#endif /*BUTTONLED_H*/
