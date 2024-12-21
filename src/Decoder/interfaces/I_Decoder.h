/*
 * I_Decoder.h
 *
 *  Created on: 20.12.2024
 *      Author: Marc
 */

#ifndef DECODER_INTERFACE_I_DECODER_H_
#define DECODER_INTERFACE_I_DECODER_H_

#include <iostream>
#include <stdexcept>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <thread>
#include <chrono>


#include "../../HAL/headers/SensorISR.h"


#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"

class I_Decoder : public PulseMsgHandler {
  public:
    //Decoder(const std::string dispatcherChannelName);
    virtual ~I_Decoder() {};

    virtual void handleMsg() override = 0;  
    virtual void sendMsg() override = 0; 
    virtual int32_t getChannel() override = 0; 

  private:
    virtual void decode() = 0;

  private:
    // bool running;
    // int32_t channelID;
    // SensorISR *sensorISR;
    // int32_t dispatcherConnectionID;
};



#endif /* DECODER_INTERFACE_I_DECODER_H_ */
