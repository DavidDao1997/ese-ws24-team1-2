/*
 * Decoder.h
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#ifndef HEADER_DECODER_H_
#define HEADER_DECODER_H_

#include <iostream>
#include <stdexcept>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <thread>

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"

class Decoder : public PulseMsgHandler {
  public:
    Decoder(const std::string channelName, const std::string dispatcherChannelName);
    ~Decoder();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

  private:
    void handleInterrupt();

  private:
    int32_t channelID;
    name_attach_t *decoderChannel;
};

#endif /* HEADER_DECODER_H_ */
