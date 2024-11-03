/*
 * HALConfig.h
 *
 *  Created on: 30.10.2024
 *      Author: Marc
 */

#ifndef HALCONFIG_H_
#define HALCONFIG_H_


// GPIO Banks
#define GPIO_BANK_0 0x44E07000
#define GPIO_BANK_1 0x4804C000
#define GPIO_BANK_2 0x481AC000
#define GPIO_BANK_3 0x481AE000

#define IO_MEM_LEN 0x1000

#define GPIO_CLEAR 0x190
#define GPIO_SET 0x194

// LightBarriers
#define LBF_PIN 2
#define LBE_PIN 20
#define LBR_PIN 15
#define LBM_PIN 5

// LEDs
#define LR_PIN 16
#define LY_PIN 17
#define LG_PIN 18

#define BGSL_PIN 2
#define BRSL_PIN 3
#define Q1_PIN 4
#define Q2_PIN 5

//Motor
#define M_FORWARD_PIN 12
#define M_BACKWARD_PIN 13
#define M_SLOW_PIN 14
#define M_STOP_PIN 15

//SortingMOdule
#define SM_PIN 19


// Type to specify Pin number
typedef uint8 Pin;






#endif /* HALCONFIG_H_ */
