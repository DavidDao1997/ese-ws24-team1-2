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
// #include "../../HAL/headers/Actuators_Wrapper.h"
#include "../../Logging/headers/Logger.h"

#define ACTUATOR_CONTROLLER_NUM_OF_PULSES 36

class ActuatorController : public PulseMsgHandler {
  public:
    // TODO check if festo 1 or festo 2 in parameter list
    ActuatorController(uint8_t festo, const std::string name, I_Actuators_Wrapper *actuators);
    virtual ~ActuatorController();

    void subscribeToDispatcher();

    void handleMsg() override;
    int32_t getChannel() override;
    void sendMsg() override;

    int8_t* getPulses();
    int8_t getNumOfPulses();
    bool stop();

    bool getGreenBlinking();  
    bool getYellowBlinking();  
    bool getRedBlinking(); 
    
    static std::atomic<bool> lgblinking;
    static std::atomic<bool> lyblinking;
    static std::atomic<bool> lrblinking; 

    static int8_t numOfPulses;
    static int8_t pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES];

  private:
    int32_t channelID;
    uint8_t festoID;
    name_attach_t *actuatorControllerChannel;
    I_Actuators_Wrapper *actuators;

    bool running;
    

    

    std::thread *greenBlinkThread;
    std::thread *yellowBlinkThread;
    std::thread *redBlinkThread;

    void startYellowLampBlinkingThread(int frequency);
    void startGreenLampBlinkingThread(int frequency);
    void startRedLampBlinkingThread(int frequency);
    void greenlampBlinking(std::atomic<bool> * blink, int32_t frequency);
    void yellowlampBlinking(std::atomic<bool> * blink, int32_t frequency);
    void redlampBlinking(std::atomic<bool> * blink, int32_t frequency);
};

#endif /* ACTUATORCONTROLLER_H_ */
