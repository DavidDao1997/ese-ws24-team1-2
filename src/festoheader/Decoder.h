/*
 * Decoder.h
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#ifndef HEADER_DECODER_H_
#define HEADER_DECODER_H_

#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <iostream>
#include <thread>
#include <stdexcept>

#include "../Dispatcher/PulseMsgHandler.h"

class Decoder : public PulseMsgHandler {
public:
    Decoder(const char* name);
    ~Decoder();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;
    
private:
    void handleInterrupt();

private:

   int32_t channelID;
   name_attach_t* decoderChannel;
};



#endif /* HEADER_DECODER_H_ */
