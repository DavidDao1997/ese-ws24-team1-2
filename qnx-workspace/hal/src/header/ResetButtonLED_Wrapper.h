/*
 * StartButtonLED.h
 *
 *  Created on: 28.10.2024
 *      Author: Marc
 */

#ifndef HEADER_STARTBUTTONLED_H_
#define HEADER_STARTBUTTONLED_H_

#include "../interface/I_Led.h"

class StartButtonLED_Wrapper : I_ButtonLED {

	void switchLightOn() override;
	void switchLightOff() override;

};




#endif /* HEADER_STARTBUTTONLED_H_ */
