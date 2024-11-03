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
	I_Control(); //TODO I_Control(uint16 someident);
	virtual ~I_Control();

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
	void stopButtonLightOn();
	void stopButtonLightOn();
	void Q1LightOn();
	void Q1LightOff();
	void Q2LightOn();
	void Q2LightOff();


	void conveyorRight();
	void conveyorLeft();
	void conveyorSlow();
	void conveyorFast();

private:

	//uint16 identifuer // TODO

	//I_HeightSensor hs;
	I_Led redLamp;
	I_Led yellowLamp;
	I_Led greenLamp;
	I_Led startBtnLed;
	I_Led stopBtnLed;
	I_Led q1;
	I_Led q2;
	I_Motor motor;

};


#endif /* HEADER_I_CONTROL_H_ */
