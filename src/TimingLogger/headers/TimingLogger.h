/*
 * Dispatcher.h
 *
 *  Created on: 21.11.2024
 *      Author: Marc
 */

#ifndef TIMINGLOGGER_H
#define TIMINGLOGGER_H

#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Util/headers/Util.h"
#include "../../Logging/headers/Logger.h"
#include <chrono>



#define TIMING_LOGGER_NUM_OF_PULSES 12

class TimingLogger : public PulseMsgHandler {
  public:
    TimingLogger(std::string dispatcherChannelname);
    virtual ~TimingLogger();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    int8_t *getPulses();
    int8_t getNumOfPulses();
    bool stop();

    static int8_t numOfPulses;
    static int8_t pulses[TIMING_LOGGER_NUM_OF_PULSES];


  private:
    void subscribeToOutEvents();
    void handlePulse(_pulse msg);

    int32_t channelID;
    bool running;
    int32_t dispatcherConnectionID;

};

#endif // TIMINGLOGGER_H
