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

    void startNewTimer(Timer timer); // put Timer to List
    void startNewValidTimer(Timer timer); // do not put this Timer to List
    void updateTimersFast();
    void updateTimersSlow();
    void updateTimersStop(); // ?
    void deleteListTimers(); // not valid timer
private:
    uint32_t id;
    Timer valid; //    Valid = nullptr
    std::list<Timer> timerlist;
    
    bool heightIsValid;
    bool isMetal;
    // bool isValid; // it feals like we should calculate this just in time instead of writing a property that we need to update.
    // func updateTimers(motorState)
};



#endif /* FSM_PUK_HEADERS_PUK_H_ */
