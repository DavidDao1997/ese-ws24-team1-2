#include <iostream>
#include <string>

// #include "festoheader/Decoder.h"
#include "Dispatcher/header/Dispatcher.h"
#include "Util/headers/Util.h"
#include "festoheader/ActuatorController.h"

int main() {
    std::string dispatcherChannel = "dispatcher";
    Dispatcher dispatcher(dispatcherChannel);
    // dispatcher->addSubSbrber(int32_t coid, uint_8[])

    // string decoderChannel = "decoder";
    // Decoder decoder(decoderChannel);

    std::string actuatorControllerChannel = "actuatorController";
    Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
    ActuatorController actuatorController(actuatorControllerChannel, actuatorsWrapper);
    dispatcher.addSubscriber(
        actuatorController.getChannel(), actuatorController.pulses, actuatorController.numOfPulses
    );
    // construct actuatorControllerMaster
    // construct decoder
    // construct ... (eg FSMs, Heartbeat, EStop)

    // start thread dispatcher
    // start thread ...

    // thread.join()...

    // maybe loop and dont die or dont die
    while (1) {
        std::cout << "sleeping for 1 second" << std::endl;
        WAIT(1000);
    }
    return 0;
}
