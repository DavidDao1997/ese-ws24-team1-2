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
    Decoder(const std::string dispatcherChannelName, uint8_t festoID);
    virtual ~Decoder();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;
    bool stop();

  private:
    void decode() override;

  private:
    bool running;
    int32_t channelID;
    SensorISR *sensorISR;
    int32_t dispatcherConnectionID;
    uint8_t festoNr;
};

#endif /* HEADER_DECODER_H_ */
