/*
 * ActuatorController.h
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#ifndef ACTUATORCONTROLLER_H_
#define ACTUATORCONTROLLER_H_

#include "../Dispatcher/PulseMsgHandler.h"
#include "../HAL/halheader/Actuators_Wrapper.h"


class ActuatorController : public PulseMsgHandler{
public:
    ActuatorController(const char* name, Actuators_Wrapper* actuators);
    ~ActuatorController();


    void handleMsg() override;
    int32_t getChannel() override;
    void sendMsg() override;

private:
    
    int32_t channelID;
    name_attach_t* actConChannel;
    Actuators_Wrapper* actuators;
};




#endif /* ACTUATORCONTROLLER_H_ */
