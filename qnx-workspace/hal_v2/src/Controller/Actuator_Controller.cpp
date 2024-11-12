/*
 * Actuator_Controller.cpp
 *
 *  Created on: 11.11.2024
 *      Author: Marc
 */

#include "Actuator_Controller.h"




Actuator_Controller::Actuator_Controller(){
	Actuators_Wrapper* festoActuators = new Actuators_Wrapper();
	sortingModule = 0;
}


Actuator_Controller::~Actuator_Controller(){
	delete festoActuators;
}


bool Actuator_Controller::init(){
	if (!festoActuators->init()){
		return false;
	}
	sortingModule = festoActuators->readSortingModule();
	return true;
}










