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

#include "../../Logging/headers/Logger.h"
class HeartBeat : public PulseMsgHandler {
  public:
    HeartBeat(uint8_t festoNrOfOtherFesto, std::string dispatcherName);
    virtual ~HeartBeat();

    bool stop();
    int32_t getChannel() override;
    void sendMsg() override;
    void handleMsg() override;

  private:
    void handleDisconnect(bool disconnect);
    bool running;
    bool eStopPressed;
    int32_t channelID;
    int32_t festoId;
    int32_t otherFesto;
    int32_t dispatcherChannel;
    name_attach_t* heartBeatChannel;
    std::mutex heartbeatMutex;
    std::chrono::steady_clock::time_point lastHeartbeatReceived;
};

/*
    Master als auch Slave senden synchrone Pulsemsgs in einem definierten Zeitraum
    Wenn Nachricht angekommen sleep for weitere 100 ms (?) dann neue Nachricht.
    Wenn nicht erfolgreiche antwort, dann E-Stop verhalten.

 */

#endif /* HEARTBEAT_H_ */
