/*
 * PulseMsgConfig.h
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#ifndef DISPATCHER_PULSEMSGCONFIG_H_
#define DISPATCHER_PULSEMSGCONFIG_H_

typedef struct PulseEventMsg {
	int32_t type;
	int32_t coid;
	void* payload;
} PulseEventMsg_t ;

// typedef struct PulseEventMsgV2 {
// 	uint8_t type;
// 	uint32_t payload;
// } PulseEventMsgV2_t ;

// typedef struct PulseSubscribe {
// 	uint8_t type;
// 	struct payload {
//         coid
//     };
// } PulseEventMsgV2_t ;

// PulseCodes GENERAL
#define PULSE_STOP_THREAD _PULSE_CODE_MINAVAIL + 1

// PULSECODE DECODER
#define PULSE_INTR_ON_PORT0 _PULSE_CODE_MINAVAIL + 2

// Pulse Codes HEARTBEAT
#define PULSE_HEARTBEAT _PULSE_CODE_MINAVAIL + 3

// PULSECODES DISPATCHER
#define PULSE_INTR_ON_ESTOP _PULSE_CODE_MINAVAIL + 4
// #define PULSE_INTR_ON_ESTOP _PULSE_CODE_MINAVAIL + 5
// #define PULSE_INTR_ON_ESTOP _PULSE_CODE_MINAVAIL + 6
// #define PULSE_INTR_ON_ESTOP _PULSE_CODE_MINAVAIL + 7
// #define PULSE_INTR_ON_ESTOP _PULSE_CODE_MINAVAIL + 8
// #define PULSE_INTR_ON_ESTOP _PULSE_CODE_MINAVAIL + 9

// PULSECODES ACTUATOR CONTROLLER
#define PULSE_MOTOR_STOP _PULSE_CODE_MINAVAIL + 5
#define PULSE_MOTOR_START _PULSE_CODE_MINAVAIL + 6
#define PULSE_MOTOR_SLOW _PULSE_CODE_MINAVAIL + 7
#define PULSE_MOTOR_FAST _PULSE_CODE_MINAVAIL + 8




#endif /* DISPATCHER_PULSEMSGCONFIG_H_ */
