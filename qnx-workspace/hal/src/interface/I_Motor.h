/*
 * Motor.h
 *
 *  Created on: 22.10.2024
 *      Author: Marc
 */

#ifndef INTERFACE_I_MOTOR_H_
#define INTERFACE_I_MOTOR_H_

class I_Motor {
public:
	virtual void setRight() = 0;
	virtual void setLeft() = 0;
	virtual void setStop() = 0;
	virtual void setSlow() = 0;
	virtual void setFast() = 0;

	virtual ~I_Motor();
};



#endif /* INTERFACE_I_MOTOR_H_ */
