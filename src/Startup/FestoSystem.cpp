/*
 * FestoSystem.cpp
 *
 *  Created on: 06.01.2025
 *      Author: Marc
 */

#include "headers/FestoSystem.h"

FestoSystem::FestoSystem(uint8_t festo){
    festoId = festo;

    if (festo == FESTO1) {
        std::string actuatorControllerChannelName = "actuatorController1";
        std::string heightControllerChannelName = "HSControl1"
        dispatcher = new Dispatcher(DISPATCHERNAME);
        FSMController *fsmController = new FSMController(dispatcherChannelName);


        dispatcher->addSubscriber(
            fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        );
    
    } else if (feso == FESTO2) {
        std::string actuatorControllerChannelName = "actuatorController2";
        std::string heightControllerChannelName = "HSControl2"
    } else {
        logger.log(LogLevel::CRITICAL, "Cannot resolve Festo Nr...", "FestoSystem");
    }
    decoder = new Decoder(DISPATCHERNAME, festoId);
    Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
    ActuatorController *actuatorController = new ActuatorController(actuatorControllerChannelName, actuatorsWrapper);

    TSCADC* tsc = new TSCADC();
    ADC* adc = new ADC(*tsc);    
    HeightSensorControl *heightSensorController = new HeightSensorControl(heightControllerChannelName, dispatcherChannelName, FESTO1, tsc, adc);
    
    dispatcher->addSubscriber(
        actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
    );

    
    
    


}

void FestoSystem::startUp(){
    
}

void FestoSystem::stop(){

}


FestoSystem::~FestoSystem(){
    

}