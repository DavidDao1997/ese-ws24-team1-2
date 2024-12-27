/*
 * PulseMsgHandler.h
 * Interface class to send and receive PulseMessages
 *
 *  Created on: 30.11.2024
 *      Author: Marc
 */

#ifndef PULSEMSGHANDLER_H_
#define PULSEMSGHANDLER_H_

#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <sys/dispatch.h>
#include <sys/iofunc.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <thread>

#include "../../Logging/headers/Logger.h"

/* FOR REFERECE
typedef struct _name_attach {
    dispatch_t* dpp;        The dispatch handle used in the creation of this connection.
    int         chid;       The channel ID used for MsgReceive() directly.
    int         mntid;      the mount ID for this name.
    int         zero[2];
} name_attach_t;
*/

class PulseMsgHandler {
    // methods that can be overriden, no need to implement
  public:
    static int32_t createChannel();
    static name_attach_t *createNamedChannel(const std::string channelName);
    static void destroyChannel(uint32_t channelID);
    static void destroyNamedChannel(int32_t channelID, name_attach_t *attach);
    static int32_t connectToChannel(int32_t connectChannelID);
  
    // methods needed to be implemented
  public:
    virtual void handleMsg() = 0;
    virtual void sendMsg() = 0;
    virtual int32_t getChannel() = 0;

  private:
    // local class variables/fields etc
  private:
};

#endif /* PULSEMSGHANDLER_H_ */
