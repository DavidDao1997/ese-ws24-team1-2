/*
 * I_Control.cpp
 *
 *  Created on: 03.11.2024
 *      Author: Marc
 */


#include <stdint.h>
#include "sys/mman.h"
#include "sys/neutrino.h"
#include "interface/I_Control.h"
#include "HALConfig.h"


I_Control::I_Control(){
	redLamp = new I_Led(LR_PIN);
	yellowLamp = new I_Led(LY_PIN);
	greenLamp = new I_Led(LG_PIN);

	startBtnLed = new I_Led(BGSL_PIN);
	stopBtnLed = new I_Led(BRSL_PIN);
	q1 = new I_Led(Q1_PIN);
	q2 = new I_Led(Q2_PIN);

	motor = new I_Motor();

}

bool I_Control::init(){

	uintptr_t gpio_bank_0 = mmap_device_io(IO_MEM_LEN, (uint64_t) GPIO_BANK_0);
	if (MAP_DEVICE_FAILED == gpio_bank_0){
		return false;
	}

	uintptr_t gpio_bank_1 = mmap_device_io(IO_MEM_LEN, (uint64_t) GPIO_BANK_1);
	if (MAP_DEVICE_FAILED == gpio_bank_1){
			return false;
		}
	uintptr_t gpio_bank_2 = mmap_device_io(IO_MEM_LEN, (uint64_t) GPIO_BANK_2);
	if (MAP_DEVICE_FAILED == gpio_bank_2){
			return false;
		}

	uintptr_t gpio_bank_3 = mmap_device_io(IO_MEM_LEN, (uint64_t) GPIO_BANK_3);
	if (MAP_DEVICE_FAILED == gpio_bank_3){
			return false;
		}


	return true;

}


void I_Control::redLampLightOn(){
	redLamp.switchLightOn();
}

void I_Control::redLampLightOff(){
	redLamp.switchLightOff();
}

void I_Control::yellowLampLightOn(){
	yellowLamp.switchLightOn();
}

void I_Control::yellowLampLightOff(){
	yellowLamp.switchLightOff();
}

void I_Control::greenLampLightOn(){
	greenLamp.switchLightOn();
}

void I_Control::greenLampLightOff(){
	greenLamp.switchLightOff();
}


void I_Control::startButtonLightOn(){
	startBtnLed.switchLightOn();
}

void I_Control::startButtonLightOff(){
	startBtnLed.switchLightOff();
}

void I_Control::stopButtonLightOn(){
	stopBtnLed.switchLightOn();
}

void I_Control::stopButtonLightOff(){
	stopBtnLed.switchLightOff();
}

void I_Control::Q1LightOn(){
	q1.switchLightOn();
}

void I_Control::Q1LightOff(){
	q1.switchLightOff();
}

void I_Control::Q2LightOn(){
	q2.switchLightOn();
}

void I_Control::Q2LightOff(){
	q2.switchLightOff();
}


void I_Control::conveyorRight(){
	motor.setRight();
}

void I_Control::conveyorLeft(){
	motor.setLeft();
}

void I_Control::conveyorFast(){
	motor.setFast();
}

void I_Control::conveyorSlow(){
	motor.setSlow();
}





