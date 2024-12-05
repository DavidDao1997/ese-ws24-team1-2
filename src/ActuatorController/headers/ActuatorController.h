/*
 * ActuatorController.h
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#ifndef ACTUATORCONTROLLER_H_
#define ACTUATORCONTROLLER_H_

#include <string>

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../HAL/headers/Actuators_Wrapper.h"

#define ACTUATOR_CONTROLLER_NUM_OF_PULSES 4

class ActuatorController : public PulseMsgHandler {
  public:
    // TODO check if festo 1 or festo 2 in parameter list
    ActuatorController(const std::string name, Actuators_Wrapper *actuators);
    ~ActuatorController();

    void handleMsg() override;
    int32_t getChannel() override;
    void sendMsg(int8_t msgCode, int32_t msgValue) override;

    static int8_t numOfPulses;
    static int8_t pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES];

  private:
    int32_t channelID;
    name_attach_t *actConChannel;
    Actuators_Wrapper *actuators;
};

#endif /* ACTUATORCONTROLLER_H_ */
