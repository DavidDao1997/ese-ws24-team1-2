#include <functional> // For std::bind
#include <iostream>
#include <string>
#include <thread>

#include "Actuatorcontroller/headers/ActuatorController.h"
#include "Dispatcher/headers/Dispatcher.h"
#include "Logik/headers/FSM.h"
#include "Util/headers/Util.h"
#include "HeightController/header/HeightSensorControl.h"
#include "Decoder/headers/Decoder.h"
#include "Logik/FSM/headers/FSMController.h"

int main() {
    std::string dispatcherChannelName = "dispatcher";
    Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);
    // dispatcher->addSubSbrber(int32_t coid, uint_8[])

    Decoder *decoder = new Decoder(dispatcherChannelName);

    std::string actuatorControllerChannelName = "actuatorController";
    Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
    ActuatorController *actuatorController = new ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
    dispatcher->addSubscriber(
        actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
    );
    //init HS
	HeightSensorControl *heightSensorController = new HeightSensorControl(); // Create an object of HwAdcDemo
	//heightSensorController->initRoutine();
	//start Thread
    std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::initRoutine, heightSensorController));

    // std::string fsmChannelName = "fsm";
    // FSM *fsm = new FSM(fsmChannelName);
    // fsm->connectToChannel(dispatcher->getChannel());
    // dispatcher->addSubscriber(
    //     fsm->getChannel(), fsm->getPulses(), fsm->getNumOfPulses()
    // );

    FSMController *fsmController = new FSMController(dispatcherChannelName);
    dispatcher->addSubscriber(
        fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
    );

    // start threads
    std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));
    WAIT(1000);
    // std::thread fsmThread(std::bind(&FSM::handleMsg, fsm));
    std::thread fsmControllerHandleMsgThread(std::bind(&FSMController::handleMsg, fsmController));
    std::thread fsmControllerSendMsgThread(std::bind(&FSMController::sendMsg, fsmController));
    std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
    std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));

    // join threads
    std::cout << "\nThreads, started, main going idle...\n" << std::endl;
    dispatcherThread.join();
    // fsmThread.join();
    fsmControllerHandleMsgThread.join();
    fsmControllerSendMsgThread.join();
    actuatorControllerThread.join();
    decoderThread.join();
    return 0;
}
