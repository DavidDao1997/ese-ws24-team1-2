/*
 * Mock_Actuator_Wrapper.cpp
 *
 *  Created on: 08.12.2024
 *      Author: Jannik
 */

#include "hw/inout.h"
#include "sys/mman.h"
#include "sys/neutrino.h"
#include <iostream>
#include <stdint.h>
#include "Mock_Actuator_Wrapper.h"


extern int redLampOn;
extern int redLampOff;
extern int yellowLampOn;
extern int yellowLampOff;
extern int greenLampOn;
extern int greenLampOff;
extern int startButtonOn;
extern int startButtonLightOff;
extern int resButtonLightOn;
extern int resButtonLightOff;
extern int Q1LightOn;
extern int Q1LightOff;
extern int Q2LightOn;
extern int Q2LightOff;
extern int motorRight;
extern int motorLeft;
extern int motorSlow;
extern int motorFast;
extern int motorStop;
extern int readSortModule;
extern int openSortModule;
extern int closeSortModule;

Mock_Actuator_Wrapper::Mock_Actuator_Wrapper();

Mock_Actuator_Wrapper::~Mock_Actuator_Wrapper();

void redLampLightOn(){
	redLampOn++;
}
void redLampLightOff(){
	redLampOff++;
}
void yellowLampLightOn(){
	redLampOn++;
}
void yellowLampLightOff(){
	yellowLampOn++;
}
void greenLampLightOn(){
	greenLampOn++;
}
void greenLampLightOff(){
	greenLampOff++;
}
void startButtonLightOn(){
	startButtonOn++;
}
void startButtonOff(){
	startButtonOff++;
}
void resetButtonLightOn(){
	resButtonLightOn++;
}
void resetButtonLightOff(){
	resButtonLightOff++;
}
void Q1LightOn(){
	Q1LightOff++;
}
void Q1LightOff(){
	Q1LightOff++;
}
void Q2LightOn(){
	Q2LightOn++;
}
void Q2LightOff(){
	Q2LightOn++;
}
void runRight(){
	motorRight++;
}
void runLeft(){
	motorLeft++;
}
void runSlow(){
	motorSlow++;
}
void runFast(){
	motorFast++;
}
void motorStop(){
	motorStop++;
}
uint8_t readSortingModule(){
	return 1;
}
void openSortingModule(){
	openSortModule++;
}
void closeSortingModule(){
	closeSortModule++;
}








