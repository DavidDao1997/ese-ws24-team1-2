/*
 * FestoSystem.h
 *
 *  Created on: 06.01.2025
 *      Author: Marc
 */

#ifndef STARTUP_HEADERS_FESTOSYSTEM_H_
#define STARTUP_HEADERS_FESTOSYSTEM_H_

#include "../../Logging/headers/Logger.h"
#include "../../Util/headers/Util.h"

#include "../../Actuatorcontroller/headers/ActuatorController.h"
#include "../../Dispatcher/headers/Dispatcher.h"
#include "../../Logik/headers/FSM.h"
#include "../../Util/headers/Util.h"
#include "../../HAL/headers/ADC.h"
#include "../../HAL/headers/TSCADC.h"
#include "../../HeightController/header/HeightSensorControl.h"
#include "../../Decoder/headers/Decoder.h"
#include "../../Logik/FSM/headers/FSMController.h"

// TODO main in here. this is the startup for both festos
class FestoSystem {
public:
    FestoSystem(unit8_t festo);

    void startUp();
    void stop();

    virtual ~FestoSystem();

private:
    uint8_t festoId;
    Dispatcher *dispatcher;
    Decoder *decoder;

};


#endif /* STARTUP_HEADERS_FESTOSYSTEM_H_ */
