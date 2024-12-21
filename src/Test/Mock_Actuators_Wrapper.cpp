/*
 * Mock_Actuators_Wrapper.cpp
 *
 *  Created on: 20.12.2024
 *      Author: Marc
 */

#include "headers/Mock_Actuators_Wrapper.h"

Mock_Actuators_Wrapper::Mock_Actuators_Wrapper() {
    mock_gpio_bank_actuators = 0x0;
    mock_gpio_bank_panel_lights = 0x0;
}

Mock_Actuators_Wrapper::~Mock_Actuators_Wrapper() {
    
}

// initielisierung der gpio's
bool Mock_Actuators_Wrapper::init() {
    return true;
}

/** Question for the system:

*   did stop button even have a Led?
    name definition for motor and methods should be similar!
    hal config and actuators should be similar with the names
 */

/*LED's/Lamp's set/clear*/
void Mock_Actuators_Wrapper::redLampLightOn() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_SET), BIT_MASK(LR_PIN)); }
void Mock_Actuators_Wrapper::redLampLightOff() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(LR_PIN)); }

void Mock_Actuators_Wrapper::yellowLampLightOn() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_SET), BIT_MASK(LY_PIN)); }
void Mock_Actuators_Wrapper::yellowLampLightOff() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(LY_PIN)); }

void Mock_Actuators_Wrapper::greenLampLightOn() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_SET), BIT_MASK(LG_PIN)); }
void Mock_Actuators_Wrapper::greenLampLightOff() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(LG_PIN)); }

/*Button's LED's set/clear*/

//Start Button LED
void Mock_Actuators_Wrapper::startButtonLightOn() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_SET), BIT_MASK(BGSL_PIN)); }
void Mock_Actuators_Wrapper::startButtonLightOff() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_CLEAR), BIT_MASK(BGSL_PIN)); }

//Reset Button LED
void Mock_Actuators_Wrapper::resetButtonLightOn() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_SET), BIT_MASK(BRSL_PIN)); }
void Mock_Actuators_Wrapper::resetButtonLightOff() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_CLEAR), BIT_MASK(BRSL_PIN)); }

// set/clear SignalLed's help tools
//Signal Light 1
void Mock_Actuators_Wrapper::Q1LightOn() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_CLEAR), BIT_MASK(Q1_PIN)); }
void Mock_Actuators_Wrapper::Q1LightOff() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_CLEAR), BIT_MASK(Q1_PIN)); }

//Signal Light 2
void Mock_Actuators_Wrapper::Q2LightOn() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_SET), BIT_MASK(Q2_PIN)); }
void Mock_Actuators_Wrapper::Q2LightOff() { out32((uintptr_t)(mock_gpio_bank_panel_lights + GPIO_CLEAR), BIT_MASK(Q2_PIN)); }

/* set/clear Motor*/
// clear pin to start run right Motor
void Mock_Actuators_Wrapper::runRight() {
    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_STOP_PIN));
    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_BACKWARD_PIN));

    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_SET), BIT_MASK(M_FORWARD_PIN));
}
// clear pin to start run left Motor
void Mock_Actuators_Wrapper::runLeft() {
    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_STOP_PIN));
    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_FORWARD_PIN));

    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_SET), BIT_MASK(M_BACKWARD_PIN));
}
//set pin to slow down the Motor
void Mock_Actuators_Wrapper::runSlow() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_SET), BIT_MASK(M_SLOW_PIN)); }

//clear pin to set the speed of the Motor to normal
void Mock_Actuators_Wrapper::runFast() { out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_SLOW_PIN)); }

// set pin to stop Motor
void Mock_Actuators_Wrapper::motorStop() {
    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_SLOW_PIN));
    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_BACKWARD_PIN));
    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_CLEAR), BIT_MASK(M_FORWARD_PIN));

    out32((uintptr_t)(mock_gpio_bank_actuators + GPIO_SET), BIT_MASK(M_STOP_PIN));
}
//reading Module to find out which Sorting Module is available
uint8_t Mock_Actuators_Wrapper::readSortingModule() {
    return  SM_TYPE_EJECTOR;
}

// Set/clear pin for sorting Module -> depends which sorting Module it is
void Mock_Actuators_Wrapper::openSortingModule() { // do nothing
 }
void Mock_Actuators_Wrapper::closeSortingModule() { // do nothing
 }

uintptr_t Mock_Actuators_Wrapper::getActuators(){
    return mock_gpio_bank_actuators;
}

uintptr_t Mock_Actuators_Wrapper::getPanelLights(){
    return mock_gpio_bank_panel_lights;
}