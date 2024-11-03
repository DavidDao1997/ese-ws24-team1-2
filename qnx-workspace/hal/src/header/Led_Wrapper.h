/*
 * StartButtonLED.h
 *
 *  Created on: 28.10.2024
 *      Author: Marc
 */

#ifndef HEADER_STARTBUTTONLED_H_
#define HEADER_STARTBUTTONLED_H_

#include "../interface/I_Led.h"
#include "../HALConfig.h"

class Led_Wrapper : I_Led {
public:
	Led_Wrapper(Pin pin);


	void switchLightOn() override;
	void switchLightOff() override;
	void setPulse() override;

private:
	Pin pin;
};




#endif /* HEADER_STARTBUTTONLED_H_ */
