/*
 * PositionTracker.h
 *
 *  Created on: 11.01.2025
 *      Author: Marc
 */

#ifndef FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_
#define FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_

#include "../../gen/src-gen/FSM.h"
#include "../../../Logging/headers/Logger.h"
#include "../../gen/src/sc_rxcpp.h"

class PositionTracker{
public:
    PositionTracker(FSM* fsm);
    ~PositionTracker();
    
    

private:
    /* data */   
};

class PositionTrackerObserver : public sc::rx::Observer<void> {
public:
    // Constructor that accepts a callback of type void() (a function with no parameters and no return value)
    PositionTrackerObserver(std::function<void()> callback): callback(callback) {}

    // Override the next() method to invoke the callback
    virtual void next() override {
        callback();
    }

private:
    std::function<void()> callback;
};


#endif /* FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_ */
