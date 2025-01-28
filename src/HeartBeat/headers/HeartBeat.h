/*
 * HeartBeat.h
 *
 *  Created on: 29.11.2024
 *      Author: Marc
 */

#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <thread>
#include <chrono>

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../HAL/headers/HALConfig.h"
#include "../../Util/headers/Util.h"
#include "../../HAL/headers/Actuators_Wrapper.h"

#define HEARTBEAT_NUM_OF_PULSES 2

#include "../../Logging/headers/Logger.h"
class HeartBeat : public PulseMsgHandler {
  public:
    HeartBeat(uint8_t festoNr, int32_t actuatorControllerChannelId);
    virtual ~HeartBeat();

    void connectToFesto();

    bool stop();
    int32_t getChannel() override;
    void sendMsg() override;
    void handleMsg() override;
    void checkConnection();

  private:
    void handleDisconnect();
    bool running;
    bool eStopPressed;
    int32_t channelID;
    int32_t festoId;
    int32_t otherFesto;
    int32_t actChannel;
    int32_t coidAct;
    int32_t dispatcherChannel;
    bool messageReceivedOnce;
    name_attach_t* heartBeatChannel;
    std::mutex heartbeatMutex;
    std:: mutex otherFestoMutex;
    std::chrono::steady_clock::time_point lastHeartbeatReceived;

    uint32_t sendCnt;
    std::mutex sendCntMutex;

    static int8_t numOfPulses;
    static int8_t pulses[HEARTBEAT_NUM_OF_PULSES];
};

/*
    Master als auch Slave senden synchrone Pulsemsgs in einem definierten Zeitraum
    Wenn Nachricht angekommen sleep for weitere 100 ms (?) dann neue Nachricht.
    Wenn nicht erfolgreiche antwort, dann E-Stop verhalten.

 */

#endif /* HEARTBEAT_H_ */


/*
#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <thread>
#include <chrono>

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../HAL/headers/HALConfig.h"
#include "../../Util/headers/Util.h"
#include "../../HAL/headers/Actuators_Wrapper.h"

#define HEARTBEAT_NUM_OF_PULSES 2

#include "../../Logging/headers/Logger.h"
class HeartBeat : public PulseMsgHandler {
  public:
    HeartBeat(uint8_t festoNr, int32_t actuatorControllerChannelId);
    virtual ~HeartBeat();

    void connectToFesto();

    bool stop();
    int32_t getChannel() override;
    void sendMsg() override;
    void handleMsg() override;

  private:
    void handleDisconnect();
    bool running;
    bool eStopPressed;
    int32_t channelID;
    int32_t festoId;
    int32_t otherFesto;
    int32_t actChannel;
    int32_t dispatcherChannel;
    bool messageReceivedOnce;
    name_attach_t* heartBeatChannel;
    std::mutex heartbeatMutex;
    std::chrono::steady_clock::time_point lastHeartbeatReceived;


    static int8_t numOfPulses;
    static int8_t pulses[HEARTBEAT_NUM_OF_PULSES];
};

 */