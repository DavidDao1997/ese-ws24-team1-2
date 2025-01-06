/*
 * FestoSystem.cpp
 *
 *  Created on: 06.01.2025
 *      Author: Marc
 */



FestoSystem::FestoSystem(unit8_t festo){
    festoID = festo;
    dispatcher = new Dispatcher(DISPATCHERNAME);
    decoder = new Decoder(dispatcherChannelName, festoID);
    


}

FestoSystem::startUp(){

}

FestoSystem::stop(){

}


FestoSystem::~FestoSystem(){
    

}