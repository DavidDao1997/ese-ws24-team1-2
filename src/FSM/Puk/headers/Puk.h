/*
 * Puk.h
 *
 *  Created on: 08.01.2025
 *      Author: Marc
 */

#ifndef FSM_PUK_HEADERS_PUK_H_
#define FSM_PUK_HEADERS_PUK_H_

#include <list>
#include "../../Timer/headers/Timer.h"
#include "../../../Logging/headers/Logger.h"


class Puk {
public:
    Puk(uint32_t pukId);
    ~Puk();

    uint32_t getPukId();

    void approachingIngress(Timer::MotorState motorState, Timer* expected, Timer* expired); 
    void approachingHS(Timer::MotorState motorState, Timer* distanceValid, Timer* expected, Timer* expired);
    void approachingSorting(Timer::MotorState motorState, Timer* expected, Timer* expired);
    void approachingEgress(Timer::MotorState motorState, Timer* distanceValid, Timer* expected, Timer* expired, Timer* diverterTimeout);
    void setTimers(Timer::MotorState motorState);
    void clearSegementTimers();
    void clearTimers();
    void killTimers();
    void killDiverterTimeout();

    bool getIsMetal();
    void setIsMetal(bool);
    bool getIsValid();
    void setIsValid(bool);
private:
    uint32_t id;
    Timer* ingressDistanceValid = nullptr;
    Timer* sortingDistanceValid = nullptr; 
    Timer* nextExpected = nullptr;
    Timer* nextExpired = nullptr;
    Timer* diverterTimeout = nullptr;
    
    bool isValid; // height
    bool isMetal; // metal
    // bool isValid; // it feals like we should calculate this just in time instead of writing a property that we need to update.
};


#endif /* FSM_PUK_HEADERS_PUK_H_ */
