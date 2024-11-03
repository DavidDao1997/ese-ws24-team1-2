/*
 * StopButton_Wrapper.h
 *
 *  Created on: 28.10.2024
 *      Author: Marc
 */

#ifndef HEADER_STOPBUTTON_WRAPPER_H_
#define HEADER_STOPBUTTON_WRAPPER_H_


#include "../interface/I_Button.h"
#include "HALConfig.h"

class Button_Wrapper : I_Button {
public:
	Button_Wrapper(Pin pin);


	bool getPressed() override;

private:
	Pin pin;
};


#endif /* HEADER_STOPBUTTON_WRAPPER_H_ */
