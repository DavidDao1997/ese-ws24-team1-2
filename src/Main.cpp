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
    std::cout << "1" << std::endl;
    Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);
    // dispatcher->addSubSbrber(int32_t coid, uint_8[])
    std::cout << "2" << std::endl;
    Decoder *decoder = new Decoder(dispatcherChannelName);

    std::string actuatorControllerChannelName = "actuatorController";
    Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
    ActuatorController *actuatorController = new ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
    dispatcher->addSubscriber(
        actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
    );
    std::cout << "3" << std::endl;
    //init HS
	HeightSensorControl *heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName); // Create an object of HwAdcDemo
	//heightSensorController->initRoutine();
	//start Thread
    std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));

    // std::string fsmChannelName = "fsm";
    // FSM *fsm = new FSM(fsmChannelName);
    // fsm->connectToChannel(dispatcher->getChannel());
    // dispatcher->addSubscriber(
    //     fsm->getChannel(), fsm->getPulses(), fsm->getNumOfPulses()
    // );

    FSMController *fsmController = new FSMController(dispatcherChannelName);
    std::cout << "3.5" << std::endl;
    dispatcher->addSubscriber(
        fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
    );
    std::cout << "4" << std::endl;

    // start threads
    std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));
    WAIT(1000);
    // std::thread fsmThread(std::bind(&FSM::handleMsg, fsm));
    std::thread fsmControllerHandleMsgThread(std::bind(&FSMController::handleMsg, fsmController));
    //std::thread fsmControllerSendMsgThread(std::bind(&FSMController::sendMsg, fsmController));
    std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
    std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));
    std::cout << "1" << std::endl;

    WAIT(1000);
    int32_t dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_SM1_RESTING, 0)) {
        perror("Event onPukPresentIn Failed\n");
    }

    // WAIT(1000);
    // int32_t dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_LONG, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_SHORT, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_INTERRUPTED, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_OPEN, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLE, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLING_DONE, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBM_INTERRUPTED, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBM_OPEN, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBE_INTERRUPTED, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_INTERRUPTED, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBE_OPEN, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_ESTOP_LOW, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_ESTOP_HIGH, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_SHORT, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, 0)) {
    //     perror("ups");
    // }
    // WAIT(500);

    std::cout << "5" << std::endl;
    // join threads
    std::cout << "\nThreads, started, main going idle...\n" << std::endl;


    dispatcherThread.join();
    // fsmThread.join();
    fsmControllerHandleMsgThread.join();
    //fsmControllerSendMsgThread.join();
    actuatorControllerThread.join();
    decoderThread.join();


    return 0;
}
