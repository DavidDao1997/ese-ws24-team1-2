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

    redLightBlinking = false;
    yellowLightBlinking = false;
    greenLightBlinking = false; 
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
void Mock_Actuators_Wrapper::redLampLightOn() { mock_gpio_bank_actuators |= BIT_MASK(LR_PIN); }
void Mock_Actuators_Wrapper::redLampLightOff() { mock_gpio_bank_actuators &= ~BIT_MASK(LR_PIN); }

void Mock_Actuators_Wrapper::yellowLampLightOn() { mock_gpio_bank_actuators |= BIT_MASK(LY_PIN); }
void Mock_Actuators_Wrapper::yellowLampLightOff() { mock_gpio_bank_actuators &= ~BIT_MASK(LY_PIN); }

void Mock_Actuators_Wrapper::greenLampLightOn() { 
    // std::cout << "ACTUATORSWRAPPER: LED GREEN ON" << std::endl;
    // std::cout << mock_gpio_bank_actuators << std::endl;
    mock_gpio_bank_actuators |= BIT_MASK(LG_PIN); 
    // std::cout << mock_gpio_bank_actuators << std::endl;
}
void Mock_Actuators_Wrapper::greenLampLightOff() { 
    // std::cout << "ACTUATORSWRAPPER: LED GREEN OFF" << std::endl;
    // std::cout << mock_gpio_bank_actuators << std::endl;
    mock_gpio_bank_actuators &= ~BIT_MASK(LG_PIN); 
    // std::cout << mock_gpio_bank_actuators << std::endl;
}

/*Button's LED's set/clear*/

//Start Button LED
void Mock_Actuators_Wrapper::startButtonLightOn() { mock_gpio_bank_panel_lights |= BIT_MASK(BGSL_PIN); }
void Mock_Actuators_Wrapper::startButtonLightOff() { mock_gpio_bank_panel_lights &= ~BIT_MASK(BGSL_PIN); }

//Reset Button LED
void Mock_Actuators_Wrapper::resetButtonLightOn() { mock_gpio_bank_panel_lights |= BIT_MASK(BRSL_PIN); }
void Mock_Actuators_Wrapper::resetButtonLightOff() { mock_gpio_bank_panel_lights &= ~BIT_MASK(BRSL_PIN); }

// set/clear SignalLed's help tools
//Signal Light 1
void Mock_Actuators_Wrapper::Q1LightOn() { mock_gpio_bank_panel_lights |= BIT_MASK(Q1_PIN); }
void Mock_Actuators_Wrapper::Q1LightOff() { mock_gpio_bank_panel_lights &= ~BIT_MASK(Q1_PIN); }

//Signal Light 2
void Mock_Actuators_Wrapper::Q2LightOn() { mock_gpio_bank_panel_lights |= BIT_MASK(Q2_PIN); }
void Mock_Actuators_Wrapper::Q2LightOff() {mock_gpio_bank_panel_lights &= ~BIT_MASK(Q2_PIN); }

/* set/clear Motor*/
// clear pin to start run right Motor
void Mock_Actuators_Wrapper::runRight() {
    mock_gpio_bank_actuators &= ~BIT_MASK(M_STOP_PIN);
    mock_gpio_bank_actuators &= ~BIT_MASK(M_BACKWARD_PIN);

    mock_gpio_bank_actuators |= BIT_MASK(M_FORWARD_PIN);
}
// clear pin to start run left Motor
void Mock_Actuators_Wrapper::runLeft() {
    mock_gpio_bank_actuators &= ~BIT_MASK(M_STOP_PIN);
    mock_gpio_bank_actuators &= ~BIT_MASK(M_FORWARD_PIN);

    mock_gpio_bank_actuators |= BIT_MASK(M_BACKWARD_PIN);
}
//set pin to slow down the Motor
void Mock_Actuators_Wrapper::runSlow() { mock_gpio_bank_actuators |= BIT_MASK(M_SLOW_PIN); }

//clear pin to set the speed of the Motor to normal
void Mock_Actuators_Wrapper::runFast() { mock_gpio_bank_actuators &= ~BIT_MASK(M_SLOW_PIN); }

// set pin to stop Motor
void Mock_Actuators_Wrapper::motorStop() {
    mock_gpio_bank_actuators &= ~BIT_MASK(M_SLOW_PIN);
    mock_gpio_bank_actuators &= ~BIT_MASK(M_BACKWARD_PIN);
    mock_gpio_bank_actuators &= ~BIT_MASK(M_FORWARD_PIN);

    mock_gpio_bank_actuators |= BIT_MASK(M_STOP_PIN);
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

uint32_t Mock_Actuators_Wrapper::getActuators(uint8_t PIN){
    if (PIN > 0) {
        return (mock_gpio_bank_actuators & BIT_MASK(PIN)) ? 1 : 0;
    }
    return mock_gpio_bank_actuators ? 1 : 0;
}

uint32_t Mock_Actuators_Wrapper::getPanelLights(uint8_t PIN){
    if (PIN > 0 ){
        return mock_gpio_bank_panel_lights & BIT_MASK(PIN);
    }
    return mock_gpio_bank_panel_lights;
}


void Mock_Actuators_Wrapper::toggleRedBlinking(){
    redLightBlinking = !redLightBlinking;
}

void Mock_Actuators_Wrapper::toggleYellowBlinking(){
     yellowLightBlinking = !yellowLightBlinking;
}

void Mock_Actuators_Wrapper::toggleGreenBlinking(){
     greenLightBlinking = !greenLightBlinking;
}


bool Mock_Actuators_Wrapper::redBlinking(){
    return redLightBlinking;
}
bool Mock_Actuators_Wrapper::yellowBlinking(){
    return yellowLightBlinking;
}
bool Mock_Actuators_Wrapper::greenBlinking(){
    return greenLightBlinking;
}