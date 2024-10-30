/*
 * Ejector_Wrapper.h
 *
 *  Created on: 28.10.2024
 *      Author: Marc
 */

#ifndef EJECTOR_WRAPPER_H_
#define EJECTOR_WRAPPER_H_

#include "../interface/I_SortingModule.h"
#include "HALConfig.h"


class Ejector_Wrapper : I_SortingModule {
	void setOpen() override;
	void setClosed() override;
};



#endif /* EJECTOR_WRAPPER_H_ */
