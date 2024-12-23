/*
 * Mock_Decoder.h
 *
 *  Created on: 20.12.2024
 *      Author: Marc
 */

#ifndef TEST_HEADERS_MOCK_DECODER_H_
#define TEST_HEADERS_MOCK_DECODER_H_

#include "../../Decoder/interfaces/I_Decoder.h"

class Mock_Decoder : public I_Decoder {
public:
    Mock_Decoder(const std::string dispatcherChannelName);
    virtual ~Mock_Decoder();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    void sendPulse(PulseCode code,uint8_t festoNr);

    bool stop();

private:
    void decode() override;


    

private:
    // bool running;
    int32_t channelID;
    // SensorISR *sensorISR;
    int32_t dispatcherConnectionID;
};


#endif /* TEST_HEADERS_MOCK_DECODER_H_ */
