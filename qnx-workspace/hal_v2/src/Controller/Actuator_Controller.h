/*
 * Actuator_Controller.h
 *
 *  Created on: 11.11.2024
 *      Author: Marc
 */

#ifndef CONTROLLER_ACTUATOR_CONTROLLER_H_
#define CONTROLLER_ACTUATOR_CONTROLLER_H_

#include "I_ActuatorControl.h"
#include "../header/Actuators_Wrapper.h"


class Actuator_Controller : I_ActuatorControl {

public:

	Actuator_Controller();
	~Actuator_Controller();


	bool init() = 0;

	void motorRight() = 0;
	void motorLeft() = 0;
	void motorSlow() = 0;
	void motorFast() = 0;


	void redLampOn() = 0;
	void redLampOff() = 0;
	void yellowLampOn() = 0;
	void yellowLampOff() = 0;
	void greenLampOn() = 0;
	void greenLampOff() = 0;

	void pushToRamp() = 0;


private:

	Actuators_Wrapper festoActuators;
	uint8_t sortingModule;


};


#endif /* CONTROLLER_ACTUATOR_CONTROLLER_H_ */
