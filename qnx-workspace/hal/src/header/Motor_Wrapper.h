/*
 * Motor_Wrapper.h
 *
 *  Created on: 28.10.2024
 *      Author: Marc
 */

#ifndef HEADER_MOTOR_WRAPPER_H_
#define HEADER_MOTOR_WRAPPER_H_


#include "../interface/I_Motor.h"
#include "HALConfig.h"


class Motor : I_Motor{
public:
	Motor(Pin pin);

	void setRight() override;
	void setLeft() override;
	void setSlow() override;
	void setFast() override;


private:
	Pin pin;
};

#endif /* HEADER_MOTOR_WRAPPER_H_ */
