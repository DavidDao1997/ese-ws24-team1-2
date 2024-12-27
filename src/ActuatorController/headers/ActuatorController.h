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
#include "../../HAL/interfaces/I_Actuators_Wrapper.h"
#include "../../HAL/headers/Actuators_Wrapper.h"
#include "../../Logging/headers/Logger.h"

#define ACTUATOR_CONTROLLER_NUM_OF_PULSES 32

class ActuatorController : public PulseMsgHandler {
  public:
    // TODO check if festo 1 or festo 2 in parameter list
    ActuatorController(const std::string name, I_Actuators_Wrapper *actuators);
    virtual ~ActuatorController();

    void handleMsg() override;
    int32_t getChannel() override;
    void sendMsg() override;

    int8_t* getPulses();
    int8_t getNumOfPulses();
    bool stop();

  private:
    int32_t channelID;
    name_attach_t *actuatorControllerChannel;
    I_Actuators_Wrapper *actuators;

    bool running;
    static int8_t numOfPulses;
    static int8_t pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES];

    static bool lgblinking;
    static bool lrblinking;
    static bool lyblinking;

    void startYellowLampBlinkingThread(int frequency);
    void startGreenLampBlinkingThread(int frequency);
    void startRedLampBlinkingThread(int frequency);
    void greenlampBlinking(bool* blink, int32_t frequency);
    void yellowlampBlinking(bool* blink, int32_t frequency);
    void redlampBlinking(bool* blink, int32_t frequency);
};

#endif /* ACTUATORCONTROLLER_H_ */
