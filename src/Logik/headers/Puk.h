/*
 * Puk.h
 *
 *  Created on: 02.12.2024
 *      Author: Marc
 */

#ifndef PUK_H_
#define PUK_H_

#include <stdint>

#include "../../Logging/headers/Logger.h"

class Puk {
public:
    Puk();
    ~Puk();




private:
    uint32_t id;
    uint8_t type; // TODO eg. 1: nonMetal; 2: Metal; 3: referenzstein
    int16_t height; // TODO in mm
    int16_t heights[][];
    int32_t cntSamples[];

}


#endif /* PUK_H_ */
