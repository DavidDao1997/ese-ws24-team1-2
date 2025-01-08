/*
 * FestoSystem.cpp
 *
 *  Created on: 06.01.2025
 *      Author: Marc
 */

#include "headers/FestoSystem.h"

FestoSystem::FestoSystem(uint8_t festo){
    festoId = festo;
    dispatcher = new Dispatcher(DISPATCHERNAME);
    decoder = new Decoder(DISPATCHERNAME, festoId);
    


}

void FestoSystem::startUp(){

}

void FestoSystem::stop(){

}


FestoSystem::~FestoSystem(){
    

}