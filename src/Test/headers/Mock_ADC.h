/*
 * Mock_ADC.h
 *
 *  Created on: 04.01.2025
 *      Author: Marc
 */

#ifndef TEST_HEADERS_MOCK_ADC_H_
#define TEST_HEADERS_MOCK_ADC_H_

#include <mutex>
#include <condition_variable>

#include "../../HAL/interfaces/I_ADC.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Logging/headers/Logger.h"

class Mock_ADC : public I_ADC, PulseMsgHandler{
public:
	
	Mock_ADC(uint32_t bandheight, std::function<void()> onSamplingDone);
	virtual ~Mock_ADC();

	void registerAdcISR(int connectionID, char msgType) override;
	void unregisterAdcISR(void) override;
	void sample(void) override;
	void adcDisable(void) override;

    void mockInit(int32_t hscChannelId);
	void setSample(int32_t first, int32_t second, int32_t third, int32_t firstCnt, int32_t secondCnt, int32_t thirdCnt);
	void blockUntilSamplingDone();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;
private:
	
	void init(void) override;
	void stepConfigure(unsigned int stepSel, Fifo fifo, PositiveInput positiveInpChannel) override;
	void adcEnableSequence(unsigned int steps) override;
private:
	int32_t hsCoid;

	std::mutex mutex;
	std::condition_variable cv;
	bool sampling;
	int32_t sampleVal;
	int32_t sampleCnt;

	int32_t bandHeight;
	int32_t firstHeight;
	int32_t secondHeight;
	int32_t thirdHeight;

	int32_t firstHeightCnt;
	int32_t secondHeightCnt;
	int32_t thirdHeightCnt;

	std::function<void()> onSamplingDone;
};


#endif /* TEST_HEADERS_MOCK_ADC_H_ */
