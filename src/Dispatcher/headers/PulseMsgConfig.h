/*
 * PulseMsgConfig.h
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#ifndef DISPATCHER_PULSEMSGCONFIG_H_
#define DISPATCHER_PULSEMSGCONFIG_H_

#include <sys/neutrino.h>
// #include <sys/procmgr.h>
// #include <sys/mman.h>
// #include <hw/inout.h>

enum PulseCode {
    PULSE_INTR_ON_PORT0 = _PULSE_CODE_MINAVAIL + 1,
    // Heartbeath
    PULSE_HEARTBEAT,

    // EStop
    PULSE_ESTOP_HIGH,       // msg.value int32_t (festoID)
    PULSE_ESTOP_LOW,        // msg.value int32_t (festoID)

    // LightBarriers
    PULSE_LBF_INTERRUPTED,  // msg.value int32_t (festoID)
    PULSE_LBF_OPEN,         // msg.value int32_t (festoID)
    PULSE_LBE_INTERRUPTED,  // msg.value int32_t (festoID)
    PULSE_LBE_OPEN,         // msg.value int32_t (festoID)
    PULSE_LBR_INTERRUPTED,  // msg.value int32_t (festoID)
    PULSE_LBR_OPEN,         // msg.value int32_t (festoID)
    PULSE_LBM_INTERRUPTED,  // msg.value int32_t (festoID)
    PULSE_LBM_OPEN,         // msg.value int32_t (festoID)

    // Buttons
    PULSE_BGS_SHORT,        // msg.value int32_t (festoID)
    PULSE_BGS_LONG,         // msg.value int32_t (festoID)
    PULSE_BRS_SHORT,        // msg.value int32_t (festoID)
    PULSE_BRS_LONG,         // msg.value int32_t (festoID)
    PULSE_BGR_SHORT,        // msg.value int32_t (festoID)
    PULSE_BGR_LONG,         // msg.value int32_t (festoID)

    // Actuators
    // Motor
    PULSE_MOTOR1_STOP,
    PULSE_MOTOR1_SLOW,
    PULSE_MOTOR1_FAST,
    PULSE_MOTOR2_STOP,
    PULSE_MOTOR2_SLOW,
    PULSE_MOTOR2_FAST,

    // LEDs
    PULSE_LR1_ON,
    PULSE_LR1_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LR1_OFF,
    PULSE_LY1_ON,
    PULSE_LY1_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LY1_OFF,
    PULSE_LG1_ON,
    PULSE_LG1_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LG1_OFF,
    PULSE_LR2_ON,
    PULSE_LR2_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LR2_OFF,
    PULSE_LY2_ON,
    PULSE_LY2_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LY2_OFF,
    PULSE_LG2_ON,
    PULSE_LG2_BLINKING,     // msg.value int32_t (period [ms])
    PULSE_LG2_OFF,
    PULSE_Q11_ON,
    PULSE_Q11_OFF,
    PULSE_Q12_ON,
    PULSE_Q12_OFF,

    // Sorting
    PULSE_SM1_OPEN, 
    PULSE_SM1_CLOSE,
    PULSE_SM2_OPEN,
    PULSE_SM2_CLOSE,

    // HeightSensor
    PULSE_ADC_SAMPLE,
    PULSE_HS_SAMPLE,
    PULSE_HS_SAMPLING_DONE,

    // MetalSensor
    PULSE_MS_INTERRUPED,
    PULSE_MS_OPEN,
};

// // PULSECODE DECODER
// #define PULSE_INTR_ON_PORT0 _PULSE_CODE_MINAVAIL + 1

// // Pulse Codes HEARTBEAT
// #define PULSE_HEARTBEAT _PULSE_CODE_MINAVAIL + 2

// // PULSECODES SENSORS
// #define PULSE_ESTOP_HIGH _PULSE_CODE_MINAVAIL + 3
// #define PULSE_ESTOP_LOW _PULSE_CODE_MINAVAIL + 4

// #define PULSE_LBF_INTERRUPTED _PULSE_CODE_MINAVAIL + 5
// #define PULSE_LBF_OPEN _PULSE_CODE_MINAVAIL + 6

// #define PULSE_LBE_INTERRUPTED _PULSE_CODE_MINAVAIL + 7
// #define PULSE_LBE_OPEN _PULSE_CODE_MINAVAIL + 8

// #define PULSE_LBR_INTERRUPTED _PULSE_CODE_MINAVAIL + 9
// #define PULSE_LBR_OPEN _PULSE_CODE_MINAVAIL + 10

// #define PULSE_LBM_INTERRUPTED _PULSE_CODE_MINAVAIL + 11
// #define PULSE_LBM_OPEN _PULSE_CODE_MINAVAIL + 12

// #define PULSE_BGS_HIGH _PULSE_CODE_MINAVAIL + 13
// #define PULSE_BGS_LOW _PULSE_CODE_MINAVAIL + 14

// #define PULSE_BRS_HIGH _PULSE_CODE_MINAVAIL + 15
// #define PULSE_BRS_LOW _PULSE_CODE_MINAVAIL + 16

// #define PULSE_BGR_HIGH _PULSE_CODE_MINAVAIL + 17
// #define PULSE_BGR_LOW _PULSE_CODE_MINAVAIL + 18

// // PULSECODES ACTUATOR CONTROLLER
// #define PULSE_MOTOR1_STOP _PULSE_CODE_MINAVAIL + 19
// #define PULSE_MOTOR1_START _PULSE_CODE_MINAVAIL + 20
// #define PULSE_MOTOR1_SLOW _PULSE_CODE_MINAVAIL + 21
// #define PULSE_MOTOR1_FAST _PULSE_CODE_MINAVAIL + 22

// #define PULSE_MOTOR2_STOP _PULSE_CODE_MINAVAIL + 23
// #define PULSE_MOTOR2_START _PULSE_CODE_MINAVAIL + 24
// #define PULSE_MOTOR2_SLOW _PULSE_CODE_MINAVAIL + 25
// #define PULSE_MOTOR2_FAST _PULSE_CODE_MINAVAIL + 26

// #define PULSE_LR1_ON _PULSE_CODE_MINAVAIL + 27
// #define PULSE_LR1_OFF _PULSE_CODE_MINAVAIL + 28

// #define PULSE_LY1_ON _PULSE_CODE_MINAVAIL + 29
// #define PULSE_LY1_OFF _PULSE_CODE_MINAVAIL + 30

// #define PULSE_LG1_ON _PULSE_CODE_MINAVAIL + 31
// #define PULSE_LG1_OFF _PULSE_CODE_MINAVAIL + 32

// #define PULSE_LR2_ON _PULSE_CODE_MINAVAIL + 33
// #define PULSE_LR2_OFF _PULSE_CODE_MINAVAIL + 34

// #define PULSE_LY2_ON _PULSE_CODE_MINAVAIL + 35
// #define PULSE_LY2_OFF _PULSE_CODE_MINAVAIL + 36

// #define PULSE_LG2_ON _PULSE_CODE_MINAVAIL + 37
// #define PULSE_LG2_OFF _PULSE_CODE_MINAVAIL + 38

// #define PULSE_BGSL1_ON _PULSE_CODE_MINAVAIL + 39
// #define PULSE_BGSL1_OFF _PULSE_CODE_MINAVAIL + 40

// #define PULSE_BGSL2_ON _PULSE_CODE_MINAVAIL + 41
// #define PULSE_BGSL2_OFF _PULSE_CODE_MINAVAIL + 42

// #define PULSE_Q11_ON _PULSE_CODE_MINAVAIL + 43
// #define PULSE_Q11_OFF _PULSE_CODE_MINAVAIL + 44

// #define PULSE_Q12_ON _PULSE_CODE_MINAVAIL + 45
// #define PULSE_Q12_OFF _PULSE_CODE_MINAVAIL + 46

// #define PULSE_SM1_OPEN _PULSE_CODE_MINAVAIL + 47
// #define PULSE_SM1_CLOSE _PULSE_CODE_MINAVAIL + 48

// #define PULSE_SM2_OPEN _PULSE_CODE_MINAVAIL + 49
// #define PULSE_SM2_CLOSE _PULSE_CODE_MINAVAIL + 50

// // Pulse Msg for HeightSensor
// #define PULSE_ADC_START_SAMLING _PULSE_CODE_MINAVAIL + 51


#endif /* DISPATCHER_PULSEMSGCONFIG_H_ */
