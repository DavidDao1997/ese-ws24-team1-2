/*
 * Switch_Wrapper.h
 *
 *  Created on: 28.10.2024
 *      Author: Marc
 */

#ifndef HEADER_SWITCH_WRAPPER_H_
#define HEADER_SWITCH_WRAPPER_H_


#include "../interface/I_SortingModule.h"

class Switch_Wrapper : I_SortingModule {

	bool getPucksStatus() override;

};


#endif /* HEADER_SWITCH_WRAPPER_H_ */
