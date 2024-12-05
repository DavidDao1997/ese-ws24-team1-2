#include <functional> // For std::bind
#include <iostream>
#include <string>
#include <thread>

#include "Actuatorcontroller/headers/ActuatorController.h"
#include "Dispatcher/headers/Dispatcher.h"
#include "Logik/headers/FSM.h"
#include "Util/headers/Util.h"

int main() {
    std::string dispatcherChannelName = "dispatcher";
    Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);
    // dispatcher->addSubSbrber(int32_t coid, uint_8[])

    // std::string decoderChannelName = "decoder";
    // Decoder decoder(decoderChannelName, dispatcherChannelName);

    std::string actuatorControllerChannelName = "actuatorController";
    Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
    ActuatorController *actuatorController = new ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
    dispatcher->addSubscriber(
        actuatorController->getChannel(), actuatorController->pulses, actuatorController->numOfPulses
    );

    std::string fsmChannelName = "fsm";
    FSM *fsm = new FSM(fsmChannelName);
    fsm->connectToChannel(dispatcher->getChannel());

    // start threads
    std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));
    WAIT(1000);
    std::thread fsmThread(std::bind(&FSM::handleMsg, fsm));
    std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));

    // join threads
    std::cout << "\nThreads, started, main giong idle...\n" << std::endl;
    dispatcherThread.join();
    fsmThread.join();
    actuatorControllerThread.join();

    return 0;
}
