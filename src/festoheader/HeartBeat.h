/*
 * HeartBeat.h
 *
 *  Created on: 29.11.2024
 *      Author: Marc
 */

#ifndef FESTOHEADER_HEARTBEAT_H_
#define FESTOHEADER_HEARTBEAT_H_

#include <stdint.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <iostream>
#include <thread>
#include <stdexcept>

#include "../Dispatcher/header/PulseMsgHandler.h"
#include "../HAL/halheader/HALConfig.h"
#include "../Dispatcher/header/PulseMsgConfig.h"
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

    int32_t channelID;
    int32_t channelIDSecondFesto;
};

/*
    Master als auch Slave senden synchrone Pulsemsgs in einem definierten Zeitraum
    Wenn Nachricht angekommen sleep for weitere 300 ms (?) dann neue Nachricht. 
    Wenn nicht erfolgreiche antwort, dann E-Stop verhalten.

 */


#endif /* FESTOHEADER_HEARTBEAT_H_ */
