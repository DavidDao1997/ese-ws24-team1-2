/**
 * @file 		ADC.h
 * @brief		This module controls and configures the ADC of the BeagleboneBlack
 * @date 		03.11.2017
 * @author: 	T Jaehnichen
 *				HAW-Hamburg
 *          	Labor f�r technische Informatik
 *          	Berliner Tor  7
 *          	D-20099 Hamburg
 * @author:     T Lehmann
 * @version 	2
 * @details
 * @copybrief	Based on the StarterWareFree for AM335X provided by Texas Instrument
 *				
 */

#ifndef ADC_H_
#define ADC_H_



#include "../interfaces/I_ADC.h"

#include "../../Logging/headers/Logger.h"

#define ADC_TYPE   16

class ADC : public I_ADC{
public:
	ADC(void) = delete;
	ADC(TSCADC& tscadc);
	virtual ~ADC();

	void registerAdcISR(int connectionID, char msgType) override;
	void unregisterAdcISR(void) override;
	void sample(void) override;
	void adcDisable(void) override;

private:
	static const struct sigevent* adcISR(void* arg, int id);

	void init(void) override;
	void initInterrupts(void);
	void stepConfigure(unsigned int stepSel, Fifo fifo, PositiveInput positiveInpChannel) override;
	void cleanUpInterrupts(void);
	void adcEnableSequence(unsigned int steps) override;
private:
	TSCADC* tscadc;
	struct sigevent event;

	int interruptID;

};


#endif /* ADC_H_ */
