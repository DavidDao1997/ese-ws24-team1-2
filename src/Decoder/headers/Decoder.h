/*
 * Decoder.h
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#ifndef HEADER_DECODER_H_
#define HEADER_DECODER_H_

#include "../interfaces/I_Decoder.h"

class Decoder : public I_Decoder {
  public:
    Decoder(const std::string dispatcherChannelName);
    virtual ~Decoder();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

  private:
    void decode() override;

  private:
    bool running;
    int32_t channelID;
    SensorISR *sensorISR;
    int32_t dispatcherConnectionID;
};

#endif /* HEADER_DECODER_H_ */
