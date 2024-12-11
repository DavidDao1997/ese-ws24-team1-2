/*
 * HALConfig.h
 *
 *  Created on: 30.10.2024
 *      Author: Marc
 */

#ifndef HALCONFIG_H_
#define HALCONFIG_H_

#include <stdint.h>

/* GPIO port addresses (spruh73l.pdf S.177 ff.) */
// GPIO Banks
#define GPIO_BANK_0 0x44E07000
#define GPIO_BANK_1 0x4804C000
#define GPIO_BANK_2 0x481AC000
#define GPIO_BANK_3 0x481AE000

/* Helper macros */
#define BIT_MASK(x) (0x1 << (x))

/* GPIO port registers length */
#define IO_MEM_LEN 0x1000

#define GPIO_DATAIN 0x138
#define GPIO_DATAOUT 0x13

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

// Buttons LED of Start and Reset
#define BGSL_PIN 2
#define BRSL_PIN 3

//LED for Signal Light's
#define Q1_PIN 4
#define Q2_PIN 5 

//Motor
#define M_FORWARD_PIN 12
#define M_BACKWARD_PIN 13
#define M_SLOW_PIN 14
#define M_STOP_PIN 15

//SortingModule
#define SM_PIN 19
#define SM_TYPE 14
#define SM_TYPE_EJECTOR 1

//Buttons 
#define BGS_PIN 22  // Start Button (active high)
#define BRS_PIN 23  // Stop Button (active low)
#define BGR_PIN 26  // Reset Button (active high)

// Switches
#define SES_PIN 27  // EStop Switch (active low)

// Metal Sensor
#define MS_PIN 7    // High when True


// Type to specify Pin number
typedef uint8_t Pin;


// ----------------------------------------------------------------------TO BE CHECKED---------------------------------------------------------
// is ok for me DD,

// Maybe need a double check

/* Interrupt numbers  (spruh73l.pdf S.465 ff.) */
#define INTR_GPIO_PORT0 97
#define INTR_GPIO_PORT1 99
#define INTR_GPIO_PORT2 33


/* GPIO register offsets (spruh73l.pdf S.4877) */
#define GPIO_LEVELDETECT0 0x140
#define GPIO_LEVELDETECT1 0x144
#define GPIO_RISINGDETECT 0x148
#define GPIO_FALLINGDETECT 0x14C

#define GPIO_IRQSTATUS_0 0x2C
#define GPIO_IRQSTATUS_1 0x30
#define GPIO_IRQSTATUS_SET_0 0x34
#define GPIO_IRQSTATUS_SET_1 0x38


#endif /* HALCONFIG_H_ */
