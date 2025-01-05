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
#include "headers/Mock_ADC.h"

#include "../Dispatcher/headers/Dispatcher.h"
#include "../Logik/headers/FSM.h"
#include "../Util/headers/Util.h"
#include "../HAL/headers/ADC.h"
#include "../HAL/headers/TSCADC.h"
#include "../HeightController/header/HeightSensorControl.h"
#include "../Decoder/headers/Decoder.h"
#include "../Logik/FSM/headers/FSMController.h"
#include "../Logging/headers/Logger.h"


class DISABLED_SystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::getInstance().log(LogLevel::INFO, "Setup SystemTests...", "SystemTest");
        std::string dispatcherChannelName = "dispatcher";
        dispatcher = new Dispatcher(dispatcherChannelName);
        decoder = new Mock_Decoder(dispatcherChannelName);
        
        std::string actuatorControllerChannelName = "actuatorController";
        actuatorsWrapper = new Mock_Actuators_Wrapper();
        
        actuatorController = new Mock_ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
        dispatcher->addSubscriber(
            actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
        );

        TSCADC* tsc = new TSCADC();
        ADC* adc = new ADC(*tsc);
        heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName, FESTO1, tsc, adc); // Create an object of HwAdcDemo


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
        Logger::getInstance().log(LogLevel::INFO, "Tear Down Tests...", "Testing");
        if (!fsmController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop fsmControllerHandleMsgThread has not ended...", "SystemTest");}
        //if (!decoder->stop()) {std::cout << "loop decoderThread has ended" << std::endl;} // MOCK DOESNT HAVE A MSGHANDLER
        if (!actuatorController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop actuatorControllerThread has not ended...", "SystemTest");}
        if (!heightSensorController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop heightSensorControllerThread has not ended...", "SystemTest");}
        if (!dispatcher->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop dispatcherThread has not ended...", "SystemTest");}


        Logger::getInstance().log(LogLevel::INFO, "ENDING TREADS...", "SystemTest");
        // if (fsmControllerHandleMsgThread.joinable()) 
        fsmControllerHandleMsgThread.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread fsmControllerHandleMsgThread Ended...", "SystemTest");
        // if (decoderThread.joinable()) 
        decoderThread.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread decoderThread Ended...", "SystemTest");
        // if (actuatorControllerThread.joinable()) 
        actuatorControllerThread.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread actuatorControllerThread Ended...", "SystemTest");
        // if (heightSensorControllerThread.joinable()) 
        heightSensorControllerThread.join();  
        Logger::getInstance().log(LogLevel::INFO, "Thread heightSensorControllerThread Ended...", "SystemTest"); 
        // if (dispatcherThread.joinable()) 
        dispatcherThread.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread dispatcherThread Ended...", "SystemTest");

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
* TODO Aenderungen vom Mock Actuator Controller in den ActuatorController uebernehemn!!!!! am besten als init im actuator controller
*/
TEST_F(DISABLED_SystemTest, startupWithoutUsingServiceModeOnFESTO1) {
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



TEST_F(DISABLED_SystemTest, motorsStartWhenLBFInterruptOnFESTO1) {
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


TEST_F(DISABLED_SystemTest, eStopAfterServiceModeOnFESTO1) {
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

TEST_F(DISABLED_SystemTest, errorWhenPukDistanceTooShortAtFront) {
	startupSequenceNoServiceMode();
	// Puk in Front
	decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
	WAIT(500);
	decoder->sendPulse(PULSE_LBF_OPEN, 0);
	WAIT(100);
	decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
	EXPECT_EQ(actuatorsWrapper->greenBlinking(),false);
	EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
	EXPECT_EQ(actuatorsWrapper->yellowBlinking(),false);
	EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),1);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);

}

TEST_F(DISABLED_SystemTest, mototrStopsWhenPukAtEnd) {
	startupSequenceNoServiceMode();
	// Puk in Front
	decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
	WAIT(500);
	decoder->sendPulse(PULSE_LBF_OPEN, 0);
	WAIT(500);



	EXPECT_EQ(actuatorsWrapper->greenBlinking(),false);
	EXPECT_EQ(actuatorsWrapper->redBlinking(),false);
	EXPECT_EQ(actuatorsWrapper->yellowBlinking(),false);
	EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
	EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
	EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);

}


class ModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::getInstance().log(LogLevel::INFO, "Setup ModuleTests...", "ModuleTest");
        dispatcherChannelName = "dispatcher";
        dispatcher = new Dispatcher(dispatcherChannelName);     
        dispatcherThread = std::thread(std::bind(&Dispatcher::handleMsg, dispatcher));



    }

    void TearDown() override {
        Logger::getInstance().log(LogLevel::INFO, "ENDING TREADS...", "ModuleTest");
        if (!dispatcher->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop dispatcherThread has not ended...", "ModuleTest");}

        dispatcherThread.join();

    }

    std::string dispatcherChannelName;


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

TEST_F(ModuleTest, hsContollerReceivesPulses) {
    TSCADC* tsc = new TSCADC();
    Mock_ADC* adc = new Mock_ADC();
    heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName, FESTO1, tsc, adc); 
    adc->mockInit(heightSensorController->getChannel());
    adc->setSample(2000, 2500, 2700, 2500);
    adc->setSampleCnt(10,3,2,3);
    heightSensorControllerThread = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));
    
    WAIT(3000);

    // TODO add class to receive Pulses and its values from hscontroller, and check if as expected; e.g. MOCK_HSCNTRL_RECEIVER

    // TODO add methods to Mock_ADC to be able to send different Heights

    if (!heightSensorController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop heightSensorControllerThread has not ended...", "ModuleTest");}
    heightSensorControllerThread.join();
    
}
