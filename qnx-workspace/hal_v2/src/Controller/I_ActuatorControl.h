/*
 * I_ActuatorControl.h
 *
 *  Created on: 11.11.2024
 *      Author: Marc
 */

#ifndef CONTROLLER_I_ACTUATORCONTROL_H_
#define CONTROLLER_I_ACTUATORCONTROL_H_

class I_ActuatorControl{

	virtual bool init() = 0;

	virtual void motorRight() = 0;
	virtual void motorLeft() = 0;
	virtual void motorSlow() = 0;
	virtual void motorFast() = 0;


	virtual void redLampOn() = 0;
	virtual void redLampOff() = 0;
	virtual void yellowLampOn() = 0;
	virtual void yellowLampOff() = 0;
	virtual void greenLampOn() = 0;
	virtual void greenLampOff() = 0;

	virtual void pushToRamp() = 0;



	virtual ~I_ActuatorControl() {};




};



#endif /* CONTROLLER_I_ACTUATORCONTROL_H_ */
