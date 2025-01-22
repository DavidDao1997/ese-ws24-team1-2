/*
 * Puk.cpp
 *
 *  Created on: 08.01.2025
 *      Author: Marc
 */

#include "headers/Puk.h"

Puk::Puk(uint32_t pukId) {
    id = pukId;
}
    
Puk::~Puk() {}


uint32_t Puk::getPukId() {
    return id;
}

void Puk::approachingIngress(Timer::MotorState motorState, Timer* expected, Timer* expired) {
    clearSegementTimers();
    nextExpected = expected;
    nextExpired = expired;
    setTimers(motorState);
}

void Puk::approachingHS(Timer::MotorState motorState, Timer* distanceValid, Timer* expected, Timer* expired) {
    clearSegementTimers();
    ingressDistanceValid = distanceValid;
    nextExpected = expected;
    nextExpired = expired;
    setTimers(motorState);
}

void Puk::approachingSorting(Timer::MotorState motorState, Timer* expected, Timer* expired) {
    clearSegementTimers();
    nextExpected = expected;
    nextExpired = expired;
    setTimers(motorState);
}

void Puk::approachingEgress(Timer::MotorState motorState, Timer* distanceValid, Timer* expected, Timer* expired, Timer* _diverterTimeout) {
    clearSegementTimers();
    sortingDistanceValid = distanceValid;
    nextExpected = expected;
    nextExpired = expired;
    diverterTimeout = _diverterTimeout;
    setTimers(motorState);
    if (diverterTimeout != nullptr) diverterTimeout->setMotorState(Timer::MotorState::MOTOR_FAST); // this timer gets started immediatly and only gets killed but never altered
}

void Puk::killTimers() {
    if (ingressDistanceValid != nullptr) ingressDistanceValid->kill();
    if (sortingDistanceValid != nullptr) sortingDistanceValid->kill();
    if (nextExpected != nullptr) nextExpected->kill();
    if (nextExpired != nullptr) nextExpired->kill();
    if (diverterTimeout != nullptr) diverterTimeout->kill();
}

void Puk::killDiverterTimeout() {
    if (diverterTimeout != nullptr) diverterTimeout->kill();
}

void Puk::setTimers(Timer::MotorState motorState) {
    if (ingressDistanceValid != nullptr) ingressDistanceValid->setMotorState(motorState);
    if (sortingDistanceValid != nullptr) sortingDistanceValid->setMotorState(motorState);
    if (nextExpected != nullptr) nextExpected->setMotorState(motorState);
    if (nextExpired != nullptr) nextExpired->setMotorState(motorState);
}

void Puk::clearSegementTimers(){ 
    if (nextExpected != nullptr) nextExpected->kill();
    nextExpected = nullptr;
    if (nextExpired != nullptr) nextExpired->kill();
    nextExpired = nullptr;
    if (sortingDistanceValid != nullptr) sortingDistanceValid->kill();
    sortingDistanceValid = nullptr;
}

void Puk::clearTimers() {
    if (ingressDistanceValid != nullptr) ingressDistanceValid->kill();
    ingressDistanceValid = nullptr;
    clearSegementTimers();
}

bool Puk::getIsMetal() {
    return isMetal;
}

void Puk::setIsMetal(bool _isMetal) {
    isMetal = _isMetal;
}

bool Puk::getIsValid() {
    return isValid;
}

void Puk::setIsValid(bool _isValid) {
    isValid = _isValid;
}
