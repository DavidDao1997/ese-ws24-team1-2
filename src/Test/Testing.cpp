/*
 * Testing.cpp
 *
 *  Created on: 11.12.2024
 *      Author: Marc
 */
#include <gtest/gtest.h>

#include <functional> // For std::bind
#include <string>
#include <thread>

#include "headers/Mock_Actuators_Wrapper.h"
#include "headers/Mock_Decoder.h"
#include "headers/Mock_ActuatorController.h"

#include "../Dispatcher/headers/Dispatcher.h"
#include "../Logik/headers/FSM.h"
#include "../Util/headers/Util.h"
#include "../HeightController/header/HeightSensorControl.h"
#include "../Decoder/headers/Decoder.h"
#include "../Logik/FSM/headers/FSMController.h"


class TestEnvironment : public ::testing::Test {
protected:
    void SetUp() override {
        std::string dispatcherChannelName = "dispatcher";
        dispatcher = new Dispatcher(dispatcherChannelName);
        decoder = new Mock_Decoder(dispatcherChannelName);
        
        std::string actuatorControllerChannelName = "actuatorController";
        actuatorsWrapper = new Mock_Actuators_Wrapper();
        
        actuatorController = new Mock_ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
        dispatcher->addSubscriber(
            actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
        );

        heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName); // Create an object of HwAdcDemo


        heightSensorControllerThread = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));

        fsmController = new FSMController(dispatcherChannelName);
        dispatcher->addSubscriber(
            fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        );

        dispatcherThread = std::thread(std::bind(&Dispatcher::handleMsg, dispatcher));
        WAIT(1000);
        fsmControllerHandleMsgThread = std::thread(std::bind(&FSMController::handleMsg, fsmController));
        actuatorControllerThread = std::thread(std::bind(&Mock_ActuatorController::handleMsg, actuatorController));
        decoderThread = std::thread(std::bind(&Mock_Decoder::handleMsg, decoder));
        WAIT(1000);
    }

    void TearDown() override {
        if (!fsmController->stop()) {std::cout << "loop fsmControllerHandleMsgThread has ended" << std::endl;}
        //if (!decoder->stop()) {std::cout << "loop decoderThread has ended" << std::endl;} // MOCK DOESNT HAVE A MSGHANDLER
        if (!actuatorController->stop()) {std::cout << "loop actuatorControllerThread has ended" << std::endl;}
        if (!heightSensorController->stop()) {std::cout << "loop heightSensorControllerThread has ended" << std::endl;}
        if (!dispatcher->stop()) {std::cout << "loop dispatcherThread has ended" << std::endl;}


        
        std::cout << "ENDING TREADS" << std::endl;
        // if (fsmControllerHandleMsgThread.joinable()) 
        fsmControllerHandleMsgThread.join();
        std::cout << "Tread fsmControllerHandleMsgThread Ended" << std::endl;
        // if (decoderThread.joinable()) 
        decoderThread.join();
        std::cout << "Tread decoderThread Ended" << std::endl;
        // if (actuatorControllerThread.joinable()) 
        actuatorControllerThread.join();
        std::cout << "Tread actuatorControllerThread Ended" << std::endl;
        // if (heightSensorControllerThread.joinable()) 
        heightSensorControllerThread.join();   
        std::cout << "Tread heightSensorControllerThread Ended" << std::endl;
        // if (dispatcherThread.joinable()) 
        dispatcherThread.join();
        std::cout << "Tread dispatcherThread Ended" << std::endl;

        delete heightSensorController;
		delete decoder;
		delete fsmController;
		delete actuatorController;
		delete actuatorsWrapper;
        delete dispatcher;


    }

    void startupSequenceNoServiceMode(){
        decoder->sendPulse(PULSE_BGS_LONG, 0);
        WAIT(500);
        decoder->sendPulse(PULSE_BRS_SHORT, 0);
        WAIT(500);
        decoder->sendPulse(PULSE_BGS_SHORT, 0);
        WAIT(500);
    }



    std::thread dispatcherThread;
    std::thread heightSensorControllerThread;
    std::thread fsmControllerHandleMsgThread;
    std::thread actuatorControllerThread;
    std::thread decoderThread;


    Dispatcher* dispatcher;
    Mock_ActuatorController* actuatorController;
    HeightSensorControl* heightSensorController;
    FSMController* fsmController;
    Mock_Decoder* decoder;
    Mock_Actuators_Wrapper* actuatorsWrapper;
};


//EXPECT_EQ(actuatorsWrapper->getActuators(0),0x00); // 0 for whole bank

/*
* TODO Aenderungen vom Mock Actuator Controller in den ActuatorController uebernehemn!!!!!
*/
TEST_F(TestEnvironment, startupWithoutUsingServiceModeOnFESTO1) {
    //EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1); // TODO Explizit Setzen Bei Startup
    decoder->sendPulse(PULSE_BGS_LONG, 0);
    WAIT(2000);
    EXPECT_EQ(actuatorsWrapper->greenBlinking(),true);
    EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->yellowBlinking(),false);
    // EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
    decoder->sendPulse(PULSE_BRS_SHORT, 0);
    WAIT(500);
    EXPECT_EQ(actuatorsWrapper->greenBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->yellowBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
    // EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
    decoder->sendPulse(PULSE_BGS_SHORT, 0);
    WAIT(500);
    EXPECT_EQ(actuatorsWrapper->greenBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->yellowBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
    // EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
}



TEST_F(TestEnvironment, motorsStartWhenLBFInterruptOnFESTO1) {
    startupSequenceNoServiceMode();
    decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
    WAIT(2000);
    EXPECT_EQ(actuatorsWrapper->greenBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->yellowBlinking(),true);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),0);
    decoder->sendPulse(PULSE_LBF_OPEN, 0);
    WAIT(500);
    EXPECT_EQ(actuatorsWrapper->greenBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->yellowBlinking(),true);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),0);
}


TEST_F(TestEnvironment, eStopAfterServiceModeOnFESTO1) {
    startupSequenceNoServiceMode();
    decoder->sendPulse(PULSE_ESTOP_LOW, 0);
    WAIT(500);
    EXPECT_EQ(actuatorsWrapper->greenBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->yellowBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
    // EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
}





