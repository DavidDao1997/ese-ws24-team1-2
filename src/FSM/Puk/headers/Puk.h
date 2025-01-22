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
    enum class PUKType {
        UNKNOWN,                // <- Default type if it's not known yet
        PUK_WITH_HOLE,
        PUK_WITH_HOLE_AND_METAL,
        PUK_WITHOUT_HOLE
    };

    // Constructor uses UNKNOWN by default
    explicit Puk(uint32_t pukId);
    ~Puk();

    uint32_t getPukId() const;
    
    // Getter and Setter for PUKType
    PUKType getPukType() const;
    void setPukType(PUKType type);

    void approachingIngress(Timer::MotorState motorState, Timer* expected, Timer* expired); 
    void approachingHS(Timer::MotorState motorState, Timer* distanceValid, Timer* expected, Timer* expired);
    void approachingSorting(Timer::MotorState motorState, Timer* expected, Timer* expired);
    void approachingEgress(Timer::MotorState motorState, Timer* distanceValid, Timer* expected, Timer* expired, Timer* diverterTimeout);
    void setTimers(Timer::MotorState motorState);
    void clearSegementTimers();
    void clearTimers();
    void killTimers();
    void killDiverterTimeout();

    bool getIsMetal() const;
    void setIsMetal(bool metal);
    bool getIsValid() const;
    void setIsValid(bool valid);
    void setAverageHeight(int averageHeight);
    int getAverageHeight();
    std::string pukTypeToString(Puk::PUKType type);


private:
    uint32_t id;
    Timer* ingressDistanceValid = nullptr;
    Timer* sortingDistanceValid = nullptr; 
    Timer* nextExpected = nullptr;
    Timer* nextExpired = nullptr;
    Timer* diverterTimeout = nullptr;
    uint32_t averageHeight = 0;
    
    // Starts as UNKNOWN
    PUKType pukType = PUKType::UNKNOWN;

    bool isValid = false; 
    bool isMetal = false; 
};

#endif // FSM_PUK_HEADERS_PUK_H_

