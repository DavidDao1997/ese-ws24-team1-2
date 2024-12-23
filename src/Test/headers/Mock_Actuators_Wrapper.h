/*
 * Mock_Actuators_Wrapper.h
 *
 *  Created on: 20.12.2024
 *      Author: Marc
 */

#ifndef TEST_HEADERS_MOCK_ACTUATORS_WRAPPER_H_
#define TEST_HEADERS_MOCK_ACTUATORS_WRAPPER_H_

#include "../../HAL/interfaces/I_Actuators_Wrapper.h"


class Mock_Actuators_Wrapper : public I_Actuators_Wrapper {
public:
    Mock_Actuators_Wrapper();
	~Mock_Actuators_Wrapper();
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


//Sorting Module
	uint8_t readSortingModule() override;
	void openSortingModule() override;
	void closeSortingModule() override;


    // return to read actuators
    uint32_t getActuators(uint8_t PIN);   // for GPIO BANK 1
    uint32_t getPanelLights(uint8_t PIN); // for GPIO BANK 2


	void toggleRedBlinking();
	void toggleYellowBlinking();
	void toggleGreenBlinking();

	bool redBlinking();
	bool yellowBlinking();
	bool greenBlinking();


private:
	
	uint32_t mock_gpio_bank_actuators;
    uint32_t mock_gpio_bank_panel_lights;


	bool redLightBlinking;
    bool yellowLightBlinking;
    bool greenLightBlinking; 
	



	
};



#endif /* TEST_HEADERS_MOCK_ACTUATORS_WRAPPER_H_ */
