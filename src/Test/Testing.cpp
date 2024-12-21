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

#include "../Actuatorcontroller/headers/ActuatorController.h"
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
        
        actuatorController = new ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
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
        actuatorControllerThread = std::thread(std::bind(&ActuatorController::handleMsg, actuatorController));
        decoderThread = std::thread(std::bind(&Mock_Decoder::handleMsg, decoder));
        WAIT(1000);
    }

    void TearDown() override {
      
        
        delete heightSensorController;
        delete decoder;
        delete fsmController;
        delete actuatorController;
        delete dispatcher;
        delete actuatorsWrapper;
        
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
        // if (dispatcherThread.joinable()) 
        dispatcherThread.join();
        std::cout << "Tread dispatcherThread Ended" << std::endl;
        // if (heightSensorControllerThread.joinable()) 
        heightSensorControllerThread.join();   
        std::cout << "Tread heightSensorControllerThread Ended" << std::endl;          


    }
    std::thread dispatcherThread;
    std::thread heightSensorControllerThread;
    std::thread fsmControllerHandleMsgThread;
    std::thread actuatorControllerThread;
    std::thread decoderThread;


    Dispatcher* dispatcher;
    ActuatorController* actuatorController;
    HeightSensorControl* heightSensorController;
    FSMController* fsmController;
    Mock_Decoder* decoder;
    Mock_Actuators_Wrapper* actuatorsWrapper;
};


TEST_F(TestEnvironment, startupWithServiceMode) {
    decoder->sendPulse(PULSE_BGS_LONG, 0);
    WAIT(500);
    decoder->sendPulse(PULSE_BRS_SHORT, 0);
    WAIT(500);
    EXPECT_EQ(actuatorsWrapper->getActuators(),0x00);
    decoder->sendPulse(PULSE_BRS_SHORT, 0);
    WAIT(500);
    EXPECT_NE(actuatorsWrapper->getActuators(),0x00);
}

/*

TEST_F(TestEnvironment, test2) {
    // Beispiel: EXPECT_EQ(mockDecoder->getChannel(), 42);
}


TEST_F(TestEnvironment, test3) {
    // Beispiel: mockDecoder->sendMsg();
    // Beispiel: EXPECT_EQ(mockActuators->getLastMessage(), "Test-Nachricht von Mock_Decoder")
    
     ;
}
*/




