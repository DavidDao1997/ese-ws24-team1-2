/*
 * I_ADC.h
 *
 *  Created on: 03.01.2025
 *      Author: Marc
 */

#ifndef HAL_INTERFACES_I_ADC_H_
#define HAL_INTERFACES_I_ADC_H_


#include <sys/siginfo.h>
#include "../headers/TSCADC.h"


class I_ADC  {
public:
	virtual void registerAdcISR(int connectionID, char msgType) = 0;
	virtual void unregisterAdcISR(void) = 0;
    virtual void adcDisable(void) = 0;
    virtual void sample(void) = 0;
    
    virtual ~I_ADC() {};

private:
	virtual void init() = 0;
    void virtual stepConfigure(unsigned int stepSel, Fifo fifo, PositiveInput positiveInpChannel) = 0;
	virtual void adcEnableSequence(unsigned int steps) = 0;
	
};



#endif /* HAL_INTERFACES_I_ADC_H_ */
