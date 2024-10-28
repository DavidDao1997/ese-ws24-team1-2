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
	virtual void runRight() = 0;
	virtual void runtLeft() = 0;
	virtual void runSlow() = 0;
};



#endif /* INTERFACE_I_MOTOR_H_ */
