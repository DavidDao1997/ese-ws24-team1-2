/*
 * I_Control.h
 *
 *  Created on: 03.11.2024
 *      Author: Marc
 */

#ifndef HEADER_I_CONTROL_H_
#define HEADER_I_CONTROL_H_

// #include "../interface/I_Button.h"
// #include "../interface/I_EStop.h"
// #include "../interface/I_LightBarrier.h"
// #include "../interface/I_MetalSensor.h"

//#include "../interface/I_HeightSensor.h"
#include "../interface/I_Led.h"
#include "../interface/I_Motor.h"
#include "../interface/I_SortingModule.h"


class I_Control {
public:
	virtual ~I_Control();

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
	virtual void stopButtonLightOn() = 0;
	virtual void stopButtonLightOn() = 0;
	virtual void Q1LightOn() = 0;
	virtual void Q1LightOff() = 0;
	virtual void Q2LightOn() = 0;
	virtual void Q2LightOff() = 0;


	virtual void conveyorRight() = 0;
	virtual void conveyorLeft() = 0;
	virtual void conveyorSlow() = 0;
	virtual void conveyorFast() = 0;

};


#endif /* HEADER_I_CONTROL_H_ */
