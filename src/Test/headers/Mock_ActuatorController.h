/*
 * ActuatorController.h
 *
 *  Created on: 04.12.2024
 *      Author: Marc
 */

#ifndef ACTUATORCONTROLLER_H_
#define ACTUATORCONTROLLER_H_

#include <string>
#include <atomic>
#include <chrono>
#include "../../Util/headers/Util.h"

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../HAL/interfaces/I_Actuators_Wrapper.h"
#include "Mock_Actuators_Wrapper.h"
#include "../../Logging/headers/Logger.h"

#define ACTUATOR_CONTROLLER_NUM_OF_PULSES 32

class Mock_ActuatorController : public PulseMsgHandler {
  public:
    // TODO check if festo 1 or festo 2 in parameter list
    Mock_ActuatorController(const std::string name, Mock_Actuators_Wrapper *actuators);
    virtual ~Mock_ActuatorController();

    void handleMsg() override;
    int32_t getChannel() override;
    void sendMsg() override;

    int8_t* getPulses();
    int8_t getNumOfPulses();
    bool stop();

    static std::atomic<bool> lgblinking;
    static std::atomic<bool> lrblinking;
    static std::atomic<bool> lyblinking;

  private:
    int32_t channelID;
    name_attach_t *actuatorControllerChannel;
    Mock_Actuators_Wrapper *actuators;

    bool running;
    static int8_t numOfPulses;
    static int8_t pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES];

    // static bool lgblinking;
    // static bool lrblinking;
    // static bool lyblinking;
    std::thread *greenBlinkThread;
    std::thread *yellowBlinkThread;
    std::thread *redBlinkThread;

    void startYellowLampBlinkingThread(int frequency);
    void startGreenLampBlinkingThread(int frequency);
    void startRedLampBlinkingThread(int frequency);
    void greenlampBlinking(std::atomic<bool>* blink, int32_t frequency);
    void yellowlampBlinking(std::atomic<bool>* blink, int32_t frequency);
    void redlampBlinking(std::atomic<bool>* blink, int32_t frequency);
};

#endif /* ACTUATORCONTROLLER_H_ */
