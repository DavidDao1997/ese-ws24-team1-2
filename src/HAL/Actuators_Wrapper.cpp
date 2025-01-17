/*
    Actuators_Wrapper.cpp

    Created on: 22.10.2024
    Author: Marc Siekmann, Jannik Schön and David Dao
    @date 08.11.2024
    @version 2
    @summary{The class ist responsible for controlling the respective actuators: motor,LED,
    start and reset Button }
 */

#include "headers/Actuators_Wrapper.h"


Actuators_Wrapper::Actuators_Wrapper() {
    gpio_bank_0 = MAP_DEVICE_FAILED;
    gpio_bank_1 = MAP_DEVICE_FAILED;
    gpio_bank_1 = MAP_DEVICE_FAILED;

    if (!init()){
        Logger::getInstance().log(LogLevel::INFO, "Init actuator Wrapper Failed...", "Actuators_Wrapper");
    }
}

Actuators_Wrapper::~Actuators_Wrapper() {
    munmap_device_io(gpio_bank_0, IO_MEM_LEN);
    munmap_device_io(gpio_bank_1, IO_MEM_LEN);
    munmap_device_io(gpio_bank_2, IO_MEM_LEN);
}

// initielisierung der gpio's
bool Actuators_Wrapper::init() {

    gpio_bank_0 = mmap_device_io(IO_MEM_LEN, (uint64_t)GPIO_BANK_0);
    gpio_bank_1 = mmap_device_io(IO_MEM_LEN, (uint64_t)GPIO_BANK_1);
    gpio_bank_2 = mmap_device_io(IO_MEM_LEN, (uint64_t)GPIO_BANK_2);
    out32((uintptr_t)(gpio_bank_2 + GPIO_OE), 0x00000000);
    out32((uintptr_t)(gpio_bank_2 + GPIO_CLEAR), 0xFFFFFFFF);
    // think bubble: clarification? should it be ||?
    if (MAP_DEVICE_FAILED == gpio_bank_1 || MAP_DEVICE_FAILED == gpio_bank_2 || MAP_DEVICE_FAILED == gpio_bank_0) {
        return false;
    }
    return true;
}
/** Question for the system:

*   did stop button even have a Led?
    name definition for motor and methods should be similar!
    hal config and actuators should be similar with the names
 */

/*LED's/Lamp's set/clear*/
void Actuators_Wrapper::redLampLightOn() { out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(LR_PIN)); }
void Actuators_Wrapper::redLampLightOff() { out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(LR_PIN)); }

void Actuators_Wrapper::yellowLampLightOn() { out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(LY_PIN)); }
void Actuators_Wrapper::yellowLampLightOff() { out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(LY_PIN)); }

void Actuators_Wrapper::greenLampLightOn() { out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(LG_PIN)); }
void Actuators_Wrapper::greenLampLightOff() { out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(LG_PIN)); }

/*Button's LED's set/clear -> maybe DATA_IN and DATA_OUT PIN*/ 

//Start Button LED
void Actuators_Wrapper::startButtonLightOn() { out32((uintptr_t)(gpio_bank_2 + GPIO_SET), BIT_MASK(BGSL_PIN)); }
void Actuators_Wrapper::startButtonLightOff() { out32((uintptr_t)(gpio_bank_2 + GPIO_CLEAR), BIT_MASK(BGSL_PIN)); }

//Reset Button LED
void Actuators_Wrapper::resetButtonLightOn() { out32((uintptr_t)(gpio_bank_2 + GPIO_SET), BIT_MASK(BRSL_PIN)); }
void Actuators_Wrapper::resetButtonLightOff() { out32((uintptr_t)(gpio_bank_2 + GPIO_CLEAR), BIT_MASK(BRSL_PIN)); }

// set/clear SignalLed's help tools
//Signal Light 1
void Actuators_Wrapper::Q1LightOn() { out32((uintptr_t)(gpio_bank_2 + GPIO_SET), BIT_MASK(Q1_PIN)); }
void Actuators_Wrapper::Q1LightOff() { out32((uintptr_t)(gpio_bank_2 + GPIO_CLEAR), BIT_MASK(Q1_PIN)); }

//Signal Light 2
void Actuators_Wrapper::Q2LightOn() { out32((uintptr_t)(gpio_bank_2 + GPIO_SET), BIT_MASK(Q2_PIN)); }
void Actuators_Wrapper::Q2LightOff() { out32((uintptr_t)(gpio_bank_2 + GPIO_CLEAR), BIT_MASK(Q2_PIN)); }

/* set/clear Motor*/
// clear pin to start run right Motor
void Actuators_Wrapper::runRight() {
    out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_STOP_PIN));
    out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_BACKWARD_PIN));

    out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(M_FORWARD_PIN));
}
// clear pin to start run left Motor
void Actuators_Wrapper::runLeft() {
    out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_STOP_PIN));
    out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_FORWARD_PIN));

    out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(M_BACKWARD_PIN));
}
//set pin to slow down the Motor
void Actuators_Wrapper::runSlow() { out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(M_SLOW_PIN)); }

//clear pin to set the speed of the Motor to normal
void Actuators_Wrapper::runFast() { out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_SLOW_PIN)); }

// set pin to stop Motor
void Actuators_Wrapper::motorStop() {
    out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_SLOW_PIN));
    out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_BACKWARD_PIN));
    out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(M_FORWARD_PIN));

    out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(M_STOP_PIN));
}
//reading Module to find out which Sorting Module is available
uint8_t Actuators_Wrapper::readSortingModule() {
    uint32_t gpioIn = in32((uintptr_t)(gpio_bank_0 + GPIO_DATAIN));
    return ((gpioIn >> SM_TYPE) & 0x01);
}

// Set/clear pin for sorting Module -> depends which sorting Module it is
void Actuators_Wrapper::openSortingModule() { out32((uintptr_t)(gpio_bank_1 + GPIO_CLEAR), BIT_MASK(SM_PIN)); } // set Bit 0
void Actuators_Wrapper::closeSortingModule() { out32((uintptr_t)(gpio_bank_1 + GPIO_SET), BIT_MASK(SM_PIN)); }  //set Bit 1
