/*
 * Switch.h
 *
 *  Created on: 22.10.2024
 *      Author: Marc
 */

#ifndef INTERFACE_I_SORTINGMODULE_H_
#define INTERFACE_I_SORTINGMODULE_H_

class I_SortingModule {
public:
	virtual bool getPucksStatus() = 0;

	virtual ~I_SortingModule();
};



#endif /* INTERFACE_I_SORTINGMODULE_H_ */
