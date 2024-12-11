#include "headers/SystemTests.h"

SystemTests::SystemTests() {}
SystemTests::~SystemTests() {}
int8_t SystemTests::Run() {
    // if (0 < setup()) {
    //     throw "error during setup";
    //     return -1;
    // }
    // std::thread (runThread

    int8_t passed = 0;
    int8_t total = 3;
    if (testEstopStopsMotorAndTurnsOnLamps()) {
        std::cout << "Test: testEstopStopsMotorAndTurnsOnLamps          PASSED" << std::endl;
        passed++;
    } else {
        std::cout << "Test: testEstopStopsMotorAndTurnsOnLamps          FAILED" << std::endl;
    }
    if (testEnteringServiceModeMakesGreenLampBlink()) {
        std::cout << "Test: testEnteringServiceModeMakesGreenLampBlink  PASSED" << std::endl;
        passed++;
    } else {
        std::cout << "Test: testEnteringServiceModeMakesGreenLampBlink  FAILED" << std::endl;
    }
    if (testInitialPukTurnsOnMotor()) {
        std::cout << "Test: testInitialPukTurnsOnMotor                  PASSED" << std::endl;
        passed++;
    } else {
        std::cout << "Test: testInitialPukTurnsOnMotor                  FAILED" << std::endl;
    }

    if (0 < cleanup()) {
        throw "error during cleanup";
        return -1;
    }

    if (passed == total) {
        std::cout << "Test Suite: SystemTests                           PASSED" << std::endl;
    } else {
        std::cout << "Test Suite: SystemTests                           FAILED (" << static_cast<int>(passed)
                  << " out of " << static_cast<int>(total) << " passed)" << std::endl;
    }

    return 0;
}
int8_t SystemTests::setup() {
    // running = true;
    // std::string dispatcherChannelName = "dispatcher";
    // Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);

    // std::string actuatorControllerChannelName = "actuatorController";
    // Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
    // ActuatorController *actuatorController = new ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
    // dispatcher->addSubscriber(
    //     actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
    // );

    // FSMController *fsmController = new FSMController(dispatcherChannelName);
    // dispatcher->addSubscriber(fsmController->getChannel(), fsmController->getPulses(),
    // fsmController->getNumOfPulses());

    // std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));
    // WAIT(1000);
    // std::thread fsmControllerHandleMsgThread(std::bind(&FSMController::handleMsg, fsmController));
    // std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));

    // while (running) {
    //     WAIT(2000);
    // }
    // return 0;
}
int8_t SystemTests::cleanup() {
    running = false;
    return 0;
}
bool SystemTests::testEstopStopsMotorAndTurnsOnLamps() { return false; }
bool SystemTests::testEnteringServiceModeMakesGreenLampBlink() { return false; }
bool SystemTests::testInitialPukTurnsOnMotor() { return false; }