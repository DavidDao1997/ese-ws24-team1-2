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

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../HAL/headers/HALConfig.h"
class HeartBeat : public PulseMsgHandler {
  public:
    HeartBeat();
    ~HeartBeat();

    bool init(int32_t hbOtherFesto);
    int32_t getChannel() override;
    void sendMsg() override;
    void handleMsg() override;

  private:
    void handleDisconnect();
    bool running;
    int32_t channelID;
    int32_t channelIDSecondFesto;
};

/*
    Master als auch Slave senden synchrone Pulsemsgs in einem definierten Zeitraum
    Wenn Nachricht angekommen sleep for weitere 300 ms (?) dann neue Nachricht.
    Wenn nicht erfolgreiche antwort, dann E-Stop verhalten.

 */

#endif /* HEARTBEAT_H_ */
