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
//#include "../interface/I_Led.h"
//#include "../interface/I_Motor.h"
//#include "../interface/I_SortingModule.h"


class Festo : I_Control {
public:
	Festo(); //TODO I_Control(uint16 someident);
	virtual ~Festo();

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
	void stopButtonLightOn() override;
	void stopButtonLightOn() override;
	void Q1LightOn() override;
	void Q1LightOff() override;
	void Q2LightOn() override;
	void Q2LightOff() override;


	void conveyorRight() override;
	void conveyorLeft() override;
	void conveyorSlow() override;
	void conveyorFast() override;

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
