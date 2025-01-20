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
#include "headers/Mock_ADC.h"

#include "../Dispatcher/headers/Dispatcher.h"
//#include "../Logik/headers/FSM.h"
#include "../Util/headers/Util.h"
#include "../HAL/headers/ADC.h"
#include "../HAL/headers/TSCADC.h"
#include "../ActuatorController/headers/ActuatorController.h"
#include "../HeightController/headers/HeightSensorControl.h"
#include "../Decoder/headers/Decoder.h"
#include "../FSM/headers/FsmController.h"
#include "../Logging/headers/Logger.h"
#include "../HeartBeat/headers/HeartBeat.h"
#include "../FSM/gen/src-gen/FSM.h"

class SystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::getInstance().log(LogLevel::INFO, "Setup SystemTests...", "SystemTest");
        std::string dispatcherChannelName = "dispatcher";
        dispatcher = new Dispatcher(dispatcherChannelName);
        dispatcherThread = std::thread(std::bind(&Dispatcher::handleMsg, dispatcher));

        decoder = new Mock_Decoder(dispatcherChannelName);
        
        std::string actuatorControllerChannelName = "actuatorController1";
        actuatorsWrapper = new Mock_Actuators_Wrapper();
        
        actuatorController = new ActuatorController(FESTO1, actuatorControllerChannelName, actuatorsWrapper);
        actuatorControllerThread = std::thread(std::bind(&ActuatorController::handleMsg, actuatorController));
        actuatorController->subscribeToDispatcher();
        // dispatcher->addSubscriber(
        //     actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
        // );

        TSCADC* tsc = new TSCADC();
        ADC* adc = new ADC(*tsc);
        heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName, FESTO1, tsc, adc); // Create an object of HwAdcDemo


        heightSensorControllerThread = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));

        fsmController = new FSMController(dispatcherChannelName);
        dispatcher->addSubscriber(
            fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        );
        WAIT(1000);
        fsmControllerHandleMsgThread = std::thread(std::bind(&FSMController::handleMsg, fsmController));
        decoderThread = std::thread(std::bind(&Mock_Decoder::handleMsg, decoder));
        WAIT(1000);
    }

    void TearDown() override {
        Logger::getInstance().log(LogLevel::INFO, "Tear Down Tests...", "Testing");
        if (!fsmController->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop fsmControllerHandleMsgThread has not ended...", "SystemTest");}
        // if (!decoder->stop()) {std::cout << "loop decoderThread has ended" << std::endl;} // MOCK DOESNT HAVE A MSGHANDLER
        if (!actuatorController->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop actuatorControllerThread has not ended...", "SystemTest");}
        if (!heightSensorController->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop heightSensorControllerThread has not ended...", "SystemTest");}
        if (!dispatcher->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop dispatcherThread has not ended...", "SystemTest");}

        Logger::getInstance().log(LogLevel::TRACE, "ENDING TREADS...", "SystemTest");
        // if (fsmControllerHandleMsgThread.joinable()) 
        fsmControllerHandleMsgThread.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread fsmControllerHandleMsgThread Ended...", "SystemTest");
        // if (decoderThread.joinable()) 
        decoderThread.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread decoderThread Ended...", "SystemTest");
        // if (actuatorControllerThread.joinable()) 
        actuatorControllerThread.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread actuatorControllerThread Ended...", "SystemTest");
        // if (heightSensorControllerThread.joinable()) 
        heightSensorControllerThread.join();  
        Logger::getInstance().log(LogLevel::TRACE, "Thread heightSensorControllerThread Ended...", "SystemTest"); 
        // if (dispatcherThread.joinable()) 
        dispatcherThread.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread dispatcherThread Ended...", "SystemTest");

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
    ActuatorController* actuatorController;
    HeightSensorControl* heightSensorController;
    FSMController* fsmController;
    Mock_Decoder* decoder;
    Mock_Actuators_Wrapper* actuatorsWrapper;
};

class ModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::getInstance().log(LogLevel::INFO, "Setup ModuleTests...", "ModuleTest");
        dispatcherChannelName = "dispatcher";
        dispatcher = new Dispatcher(dispatcherChannelName);     
        dispatcherThread = std::thread(std::bind(&Dispatcher::handleMsg, dispatcher));
        //from Systemtest
        decoder = new Mock_Decoder(dispatcherChannelName);

        std::string actuatorControllerChannelName = "actuatorController1";
        actuatorsWrapper = new Mock_Actuators_Wrapper();

        actuatorController = new ActuatorController(FESTO1,actuatorControllerChannelName, actuatorsWrapper);
        dispatcher->addSubscriber(
            actuatorController->getChannel(), ActuatorController::pulses_FESTO1, ActuatorController::numOfPulses
        );
        //end of SystemTest

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
    ActuatorController* actuatorController;
    HeightSensorControl* heightSensorController;
    FSMController* fsmController;
    Mock_Decoder* decoder;
    Mock_Actuators_Wrapper* actuatorsWrapper;

    
};

//EXPECT_EQ(actuatorsWrapper->getActuators(0),0x00); // 0 for whole bank

/*
* TODO Aenderungen vom Mock Actuator Controller in den ActuatorController uebernehemn!!!!! am besten als init im actuator controller
*/

/*
TEST_F(SystemTest, startupWithoutUsingServiceModeOnFESTO1) {
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1); // TODO Explizit Setzen Bei Startup
    
    // Entry Service Mode
    decoder->sendPulse(PULSE_BGS_LONG, 0);
    WAIT(2000);

    EXPECT_EQ(actuatorController->getGreenBlinking(),true);
    EXPECT_EQ(actuatorController->getYellowBlinking(),false);
    EXPECT_EQ(actuatorController->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
    
    // Enbtry Ready
    decoder->sendPulse(PULSE_BRS_SHORT, 0);
    WAIT(500);

    EXPECT_EQ(actuatorController->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController->getYellowBlinking(),false);
    EXPECT_EQ(actuatorController->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);

    // Entry Operational
    decoder->sendPulse(PULSE_BGS_SHORT, 0);
    WAIT(500);

    EXPECT_EQ(actuatorController->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController->getYellowBlinking(),false);
    EXPECT_EQ(actuatorController->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
}

TEST_F(SystemTest, motorsStartWhenLBFInterruptOnFESTO1) {
    startupSequenceNoServiceMode();
    decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
    WAIT(2000);
    EXPECT_EQ(actuatorController->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController->getYellowBlinking(),true);
    EXPECT_EQ(actuatorController->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),0);
    decoder->sendPulse(PULSE_LBF_OPEN, 0);
    WAIT(2000);
    EXPECT_EQ(actuatorController->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController->getYellowBlinking(),true);
    EXPECT_EQ(actuatorController->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),1);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),0);
}
*/
// TEST_F(SystemTest, eStopAfterServiceModeOnFESTO1) {
// 	decoder->sendPulse(PULSE_ESTOP_HIGH, 0);
// 	decoder->sendPulse(PULSE_ESTOP_HIGH, 1);
//     //startupSequenceNoServiceMode();
//     decoder->sendPulse(PULSE_ESTOP_LOW, 0);
//     WAIT(3000);
//     EXPECT_EQ(actuatorController->getGreenBlinking(),false);
//     EXPECT_EQ(actuatorController->getYellowBlinking(),false);
//     EXPECT_EQ(actuatorController->getRedBlinking(),false);
//     EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
//     EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),1);
//     EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),1);
//     EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
//     EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
// }


/*
TEST_F(SystemTest, heartbeat){
    HeartBeat * hb1 = new HeartBeat(FESTO1);
    HeartBeat * hb2 = new HeartBeat(FESTO2);
    
    hb1->connectToFesto();
    hb2->connectToFesto();

    std::thread hb1RecvThread = std::thread(std::bind(&HeartBeat::handleMsg, hb1));
    std::thread hb2RecvThread = std::thread(std::bind(&HeartBeat::handleMsg, hb2));  
    std::thread hb1SendThread = std::thread(std::bind(&HeartBeat::sendMsg, hb1));
    std::thread hb2SendThread = std::thread(std::bind(&HeartBeat::sendMsg, hb2));    


    WAIT(1000);

    hb1->stop();

    WAIT(1000);

    Logger::getInstance().log(LogLevel::DEBUG, "Shutting down threads hb1 and hb2...", "SystemTest");
    hb1->stop();
    hb2->stop();
    Logger::getInstance().log(LogLevel::DEBUG, "Threads stopped...", "SystemTest");
    hb1RecvThread.join();
    Logger::getInstance().log(LogLevel::DEBUG, "hb1RecvThread stopped...", "SystemTest");
    hb2RecvThread.join();
    Logger::getInstance().log(LogLevel::DEBUG, "hb2RecvThread stopped...", "SystemTest");
    hb1SendThread.join();
    Logger::getInstance().log(LogLevel::DEBUG, "hb1SendThread stopped...", "SystemTest");
    hb2SendThread.join();

    Logger::getInstance().log(LogLevel::DEBUG, "hb2SendThread stopped...", "SystemTest");


}
*/
// TEST_F(SystemTest, errorWhenPukDistanceTooShortAtFront) {
// 	startupSequenceNoServiceMode();
// 	// Puk in Front
// 	decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_LBF_OPEN, 0);
// 	WAIT(100);
// 	decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
// 	EXPECT_EQ(actuatorController->getGreenBlinking(),false);
//  EXPECT_EQ(actuatorController->getYellowBlinking(),false);
//  EXPECT_EQ(actuatorController->getRedBlinking(),false);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),1);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
// }

// TEST_F(SystemTest, mototrStopsWhenPukAtEnd) {
// 	startupSequenceNoServiceMode();
// 	// Puk in Front
// 	decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_LBF_OPEN, 0);
// 	WAIT(500);
//     decoder->sendPulse(PULSE_HS1_SAMPLE, 0);
//     WAIT(500);
//     decoder->sendPulse(PULSE_HS1_SAMPLING_DONE, 0);
//     WAIT(500);
//     decoder->sendPulse(PULSE_LBM_INTERRUPTED, 0);
//     WAIT(500);
//     decoder->sendPulse(PULSE_LBM_OPEN, 0);
//     WAIT(500);
//     decoder->sendPulse(PULSE_LBE_INTERRUPTED, 0);
//     WAIT(3000);

//  EXPECT_EQ(actuatorController->getGreenBlinking(),false);
//  EXPECT_EQ(actuatorController->getYellowBlinking(),false);
//  EXPECT_EQ(actuatorController->getRedBlinking(),false);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(LG_PIN),1);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(LY_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(LR_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_FORWARD_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_SLOW_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_BACKWARD_PIN),0);
// 	EXPECT_EQ(actuatorsWrapper->getActuators(M_STOP_PIN),1);
// }

// TEST_F(ModuleTest, HeightFSMTestValidPuk) {
// 	decoder->sendPulse(PULSE_ESTOP_HIGH, 0);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_ESTOP_HIGH, 1);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_BGS_LONG,0);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_BRS_SHORT, 0);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_BGS_SHORT, 0);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_LBF_INTERRUPTED, 0);
// 	WAIT(500);
// 	decoder->sendPulse(PULSE_LBF_OPEN, 0);
// 	WAIT(500);

//    TSCADC* tsc = new TSCADC();
//    Mock_ADC* adc = new Mock_ADC();
//    heightSensorController = new HeightSensorControl("HSControl", "dispatcher", FESTO1, tsc, adc);
//    adc->mockInit(heightSensorController->getChannel());
//    adc->setSample(1000, 2200, 3150, 2200);
//    adc->setSampleCnt(10,6,35,33);
//    heightSensorControllerThread = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));

//    WAIT(3000);
//    EXPECT_EQ(actuatorsWrapper->getActuators(Q1_PIN),1);
//    //EXPECT_EQ(actuatorsWrapper->getActuators(Q2_PIN),1);

//     // // TODO add class to receive Pulses and its values from hscontroller, and check if as expected; e.g. MOCK_HSCNTRL_RECEIVER

//     // // TODO add methods to Mock_ADC to be able to send different Heights

//     if (!heightSensorController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop heightSensorControllerThread has not ended...", "ModuleTest");}
//     // heightSensorControllerThread.join();

// };


// TEST_F(ModuleTest, hsContollerReceivesPulses) {
//     TSCADC* tsc = new TSCADC();
//     Mock_ADC* adc = new Mock_ADC();
//     heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName, FESTO1, tsc, adc);
//     adc->mockInit(heightSensorController->getChannel());
//     adc->setSample(2000, 2500, 2700, 2500);
//     adc->setSampleCnt(10,3,3,3);
//     heightSensorControllerThread = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));

//     WAIT(3000);

//     // // TODO add class to receive Pulses and its values from hscontroller, and check if as expected; e.g. MOCK_HSCNTRL_RECEIVER

//     // // TODO add methods to Mock_ADC to be able to send different Heights

//     if (!heightSensorController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop heightSensorControllerThread has not ended...", "ModuleTest");}
//     // heightSensorControllerThread.join();

// }


class SystemTestTwoFesto : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::getInstance().log(LogLevel::INFO, "Setup SystemTests...", "SystemTestTwoFesto");
        std::string dispatcherChannelName = "dispatcher";
        dispatcher = new Dispatcher(dispatcherChannelName);
        dispatcherThread = std::thread(std::bind(&Dispatcher::handleMsg, dispatcher));

        decoder1 = new Mock_Decoder(dispatcherChannelName);
        decoder2 = new Mock_Decoder(dispatcherChannelName);
        
        std::string actuatorControllerChannelName1 = "actuatorController1";
        std::string actuatorControllerChannelName2 = "actuatorController2";
        actuatorsWrapper1 = new Mock_Actuators_Wrapper();
        actuatorsWrapper2 = new Mock_Actuators_Wrapper();
        
        actuatorController1 = new ActuatorController(FESTO1, actuatorControllerChannelName1, actuatorsWrapper1);
        actuatorController2 = new ActuatorController(FESTO2, actuatorControllerChannelName2, actuatorsWrapper2);
        actuatorControllerThread1 = std::thread(std::bind(&ActuatorController::handleMsg, actuatorController1));
        actuatorControllerThread2 = std::thread(std::bind(&ActuatorController::handleMsg, actuatorController2));
        actuatorController1->subscribeToDispatcher();
        actuatorController2->subscribeToDispatcher();
        
        TSCADC* tsc1 = new TSCADC();
        TSCADC* tsc2 = new TSCADC();
        adc1 = new Mock_ADC(69, [this](){
            decoder1->sendPulse(PULSE_HS1_SAMPLING_DONE, 0);
        });
        adc2 = new Mock_ADC(69, [this](){
            decoder1->sendPulse(PULSE_HS2_SAMPLING_DONE, 0);
        });

        heightSensorController1 = new HeightSensorControl("HSControl1", dispatcherChannelName, FESTO1, tsc1, adc1); 
        heightSensorController2 = new HeightSensorControl("HSControl2", dispatcherChannelName, FESTO2, tsc2, adc2); 


        heightSensorControllerThread1 = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController1));
        heightSensorControllerThread2 = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController2));

        fsmController = new FSMController(dispatcherChannelName);
        dispatcher->addSubscriber(
            fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        );
        WAIT(1000);
        Logger::getInstance().log(LogLevel::TRACE, "Starting fsmController...", "SystemTestTwoFesto");
        fsmControllerHandleMsgThread = std::thread(std::bind(&FSMController::handleMsg, fsmController));
        Logger::getInstance().log(LogLevel::TRACE, "Starting DECODER...", "SystemTestTwoFesto");
        decoderThread1 = std::thread(std::bind(&Mock_Decoder::handleMsg, decoder1));
        decoderThread2 = std::thread(std::bind(&Mock_Decoder::handleMsg, decoder2));
        WAIT(1000);
    }

    void TearDown() override {
        Logger::getInstance().log(LogLevel::INFO, "Tear Down Tests...", "SystemTestTwoFesto");
        if (!fsmController->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop fsmControllerHandleMsgThread has not ended...", "SystemTestTwoFesto");}
        // if (!decoder->stop()) {std::cout << "loop decoderThread has ended" << std::endl;} // MOCK DOESNT HAVE A MSGHANDLER
        if (!actuatorController1->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop actuatorControllerThread1 has not ended...", "SystemTestTwoFesto");}
        if (!actuatorController2->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop actuatorControllerThread1 has not ended...", "SystemTestTwoFesto");}
        if (!heightSensorController1->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop heightSensorControllerThread1 has not ended...", "SystemTestTwoFesto");}
        if (!heightSensorController2->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop heightSensorControllerThread2 has not ended...", "SystemTestTwoFesto");}
        if (!dispatcher->stop()) {Logger::getInstance().log(LogLevel::WARNING, "loop dispatcherThread has not ended...", "SystemTestTwoFesto");}

        Logger::getInstance().log(LogLevel::TRACE, "ENDING TREADS...", "SystemTestTwoFesto");
        // if (fsmControllerHandleMsgThread.joinable()) 
        fsmControllerHandleMsgThread.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread fsmControllerHandleMsgThread Ended...", "SystemTestTwoFesto");
        // if (decoderThread.joinable()) 
        decoderThread1.join();
        decoderThread2.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread decoderThread Ended...", "SystemTestTwoFesto");
        // if (actuatorControllerThread.joinable()) 
        actuatorControllerThread1.join();
        actuatorControllerThread2.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread actuatorControllerThread Ended...", "SystemTestTwoFesto");
        // if (heightSensorControllerThread.joinable()) 
        heightSensorControllerThread1.join();  
        heightSensorControllerThread2.join();  
        Logger::getInstance().log(LogLevel::TRACE, "Thread heightSensorControllerThread Ended...", "SystemTestTwoFesto"); 
        // if (dispatcherThread.joinable()) 
        dispatcherThread.join();
        Logger::getInstance().log(LogLevel::TRACE, "Thread dispatcherThread Ended...", "SystemTestTwoFesto");

        delete heightSensorController1;
        delete heightSensorController2;
		delete decoder1;
        delete decoder2;
		delete fsmController;
		delete actuatorController1;
        delete actuatorController2;
		delete actuatorsWrapper1;
        delete actuatorsWrapper2;
        delete dispatcher;
    }

    // Simulates userInput to navigate from System.Start to System.Operational
    void mockInitializationEvents() {
        decoder1->sendPulse(PULSE_ESTOP_HIGH, 0);
        WAIT(500);
        decoder2->sendPulse(PULSE_ESTOP_HIGH, 1);
        WAIT(500);
        decoder1->sendPulse(PULSE_BGS_LONG,0);
        WAIT(500);
        decoder1->sendPulse(PULSE_BRS_SHORT, 0);
        WAIT(500);
        decoder1->sendPulse(PULSE_BGS_SHORT, 0);
        WAIT(500);
    }

    void mockValidProfile(Mock_ADC* adc) {
        adc->setSample(
            2200,   3150,   2200, 
            10,     30,     20
        );
    }

    void mockInValidProfile(Mock_ADC* adc) {
        adc->setSample(
            2200,   2500,   2200, 
            10,     30,     20
        );

        // adc->setSample(
        //     2200,   4000,   2200, 
        //     10,     30,     20
        // );
    }

    // Asserts expected actuatorState. Use when entering System.Operational
    void assertSystemEntryOperational() {
        // FESTO1
        EXPECT_EQ(actuatorController1->getGreenBlinking(),false);
        EXPECT_EQ(actuatorController1->getYellowBlinking(),false);
        EXPECT_EQ(actuatorController1->getRedBlinking(),false);
        EXPECT_EQ(actuatorsWrapper1->getActuators(LG_PIN),1);
        EXPECT_EQ(actuatorsWrapper1->getActuators(LY_PIN),0);
        EXPECT_EQ(actuatorsWrapper1->getActuators(LR_PIN),0);
        EXPECT_EQ(actuatorsWrapper1->getActuators(M_FORWARD_PIN),0);
        EXPECT_EQ(actuatorsWrapper1->getActuators(M_SLOW_PIN),0);
        EXPECT_EQ(actuatorsWrapper1->getActuators(M_BACKWARD_PIN),0);
        EXPECT_EQ(actuatorsWrapper1->getActuators(M_STOP_PIN),1);

        // FESTO2
        EXPECT_EQ(actuatorController2->getGreenBlinking(),false);
        EXPECT_EQ(actuatorController2->getYellowBlinking(),false);
        EXPECT_EQ(actuatorController2->getRedBlinking(),false);
        EXPECT_EQ(actuatorsWrapper2->getActuators(LG_PIN),1);
        EXPECT_EQ(actuatorsWrapper2->getActuators(LY_PIN),0);
        EXPECT_EQ(actuatorsWrapper2->getActuators(LR_PIN),0);
        EXPECT_EQ(actuatorsWrapper2->getActuators(M_FORWARD_PIN),0);
        EXPECT_EQ(actuatorsWrapper2->getActuators(M_SLOW_PIN),0);
        EXPECT_EQ(actuatorsWrapper2->getActuators(M_BACKWARD_PIN),0);
        EXPECT_EQ(actuatorsWrapper2->getActuators(M_STOP_PIN),1);
    }

    enum MotorState {
        MOTOR_STATE_OFF,
        MOTOR_STATE_FAST,
        MOTOR_STATE_SLOW
    };

    enum LightState {
        LIGHT_STATE_OFF,
        LIGHT_STATE_ON,
        LIGHT_STATE_BLINKING
    };

    // Type aliases for better readabilty
    using GreenLightState = LightState;
    using YellowLightState = LightState;
    using RedLightState = LightState;
    using StartBtnLightState = LightState;
    using StopBtnLightState = LightState;
    using ResetBtnLightState = LightState;
    using Q1BtnLightState = LightState;
    using Q2BtnLightState = LightState;




    void assertActuatorState(
        std::string location,
        uint8_t FestoId,

        MotorState motorState,

        GreenLightState lightStateGreen = LightState::LIGHT_STATE_OFF,
        YellowLightState lightStateYellow = LightState::LIGHT_STATE_OFF,
        RedLightState lightStateRed = LightState::LIGHT_STATE_OFF,

        StartBtnLightState lightStateBGS = LightState::LIGHT_STATE_OFF,
        StopBtnLightState lightStateBRS = LightState::LIGHT_STATE_OFF,
        ResetBtnLightState lightStateBGR = LightState::LIGHT_STATE_OFF,
        Q1BtnLightState lightStateQ1 = LightState::LIGHT_STATE_OFF,
        Q2BtnLightState lightStateQ2 = LightState::LIGHT_STATE_OFF
    ) {
        ActuatorController* actuatorController = (FestoId == FESTO1) ? actuatorController1 : actuatorController2;
        Mock_Actuators_Wrapper* actuatorsWrapper = (FestoId == FESTO1) ? actuatorsWrapper1 : actuatorsWrapper2;
        std::string festoName = "FST" + std::to_string(FestoId+1);
        checkMotorState(
            location,
            festoName + ".motor.", 
            motorState,
            actuatorsWrapper->getActuators(M_FORWARD_PIN),
            actuatorsWrapper->getActuators(M_SLOW_PIN),
            actuatorsWrapper->getActuators(M_BACKWARD_PIN),
            actuatorsWrapper->getActuators(M_STOP_PIN)
        );
    
        checkLightState(location, festoName + ".greenLight.", lightStateGreen, actuatorController->getGreenBlinking(), actuatorsWrapper->getActuators(LG_PIN));
        checkLightState(location, festoName + ".yellowLight.", lightStateYellow, actuatorController->getYellowBlinking(), actuatorsWrapper->getActuators(LY_PIN));
        checkLightState(location, festoName + ".redLight.", lightStateRed, actuatorController->getRedBlinking(), actuatorsWrapper->getActuators(LR_PIN));
        checkLightState(location, festoName + ".startBtnLight.", lightStateBGS, false /* can never be blinking since it is not implemented */, actuatorsWrapper->getActuators(BGS_PIN));
        checkLightState(location, festoName + ".stopBtnLight.", lightStateBRS, false /* can never be blinking since it is not implemented */, actuatorsWrapper->getActuators(BGS_PIN));
        checkLightState(location, festoName + ".resetBtnLight.", lightStateBGR, false /* can never be blinking since it is not implemented */, actuatorsWrapper->getActuators(BGR_PIN));
        checkLightState(location, festoName + ".q1BtnLight.", lightStateQ1, false /* can never be blinking since it is not implemented */, actuatorsWrapper->getActuators(Q1_PIN));
        checkLightState(location, festoName + ".q2BtnLight.", lightStateQ2, false /* can never be blinking since it is not implemented */, actuatorsWrapper->getActuators(Q2_PIN));
    }

    void checkMotorState(
        std::string location,
        std::string prefix,
        MotorState motorState,
        int forwardPin,
        int slowPin,
        int backwardPin,
        int stopPin
    ) {
        int expectedForwardPin = 0;
        int expectedSlowPin = 0;
        int expectedBackwardPin = 0;
        int expectedStopPin = 0;
        if (motorState == MotorState::MOTOR_STATE_OFF) {
            expectedStopPin = 1;
        } else if (motorState == MotorState::MOTOR_STATE_FAST) {
            expectedForwardPin = 1;
        } else if (motorState == MotorState::MOTOR_STATE_SLOW) {
            expectedForwardPin = 1;
            expectedSlowPin = 1;
        } else {
            FAIL() << "Unexpected motorState";
        }
        if (forwardPin != expectedForwardPin) {
            ADD_FAILURE() << "[ FAILIURE ] Expected: " << (prefix) << "forwardPin" << " == " << expectedForwardPin
                << ", but got: " << (forwardPin)
                << " [" << (location) << "]";
        } 
        if (slowPin != expectedSlowPin) {
            ADD_FAILURE() << "[ FAILIURE ] Expected: " << (prefix) << "slowPin" << " == " << expectedSlowPin
                    << ", but got: " << (slowPin)
                    << " [" << (location) << "]";
        } 
        if (backwardPin != expectedBackwardPin) {
            ADD_FAILURE() << "[ FAILIURE ] Expected: " << (prefix) << "backwardPin" << " == " << expectedBackwardPin
                << ", but got: " << (backwardPin)
                << " [" << (location) << "]";
        } 
        if (stopPin != expectedStopPin) {
            ADD_FAILURE() << "[ FAILIURE ] Expected: " << (prefix) << "stopPin" << " == " << expectedStopPin
                << ", but got: " << (stopPin)
                << " [" << (location) << "]";
        } 
    }

    void checkLightState(
        std::string location,
        std::string prefix,
        LightState lightState, 
        bool blinking, 
        int pin
    ) {
        bool blinkingExpected;
        int pinExpected;
        if (lightState == LightState::LIGHT_STATE_OFF) {
            blinkingExpected = false;
            pinExpected = 0;
        } else if (lightState == LightState::LIGHT_STATE_ON) {
            blinkingExpected = false;
            pinExpected = 1;
        } else if (lightState == LightState::LIGHT_STATE_BLINKING) {
            blinkingExpected = true;
            pinExpected = 0;
        } else {
            FAIL() << "Unexpected light state!";
        }

        if (blinking != blinkingExpected) { //  << name << ": blinking is " << std::to_string(blinking) << ", expected " << std::to_string(blinkingExpected);
            ADD_FAILURE() << "[ FAILIURE ] Expected: " << (prefix) << "blinking" << " == " << blinkingExpected
                << ", but got: " << (blinking)
                << " [" << (location) << "]";
        }
        // only check pin if not blinking
        if (!blinkingExpected && pin != pinExpected) {
            // EXPECT_EQ_SILENT(pin, pinExpected, location); // << name << ": pin is " << std::to_string(pin) << ", expected " << std::to_string(pinExpected);
            ADD_FAILURE() << "[ FAILIURE ] Expected: " << (prefix) << "pin" << " == " << pinExpected
                << ", but got: " << (pin)
                << " [" << (location) << "]";
        }
    }

    
    std::thread dispatcherThread;
    std::thread heightSensorControllerThread1;
    std::thread heightSensorControllerThread2;
    std::thread fsmControllerHandleMsgThread;
    std::thread actuatorControllerThread1;
    std::thread actuatorControllerThread2;
    std::thread decoderThread1;
    std::thread decoderThread2;


    Dispatcher* dispatcher;
    ActuatorController* actuatorController1;
    ActuatorController* actuatorController2;
    Mock_ADC* adc1;
    Mock_ADC* adc2;
    HeightSensorControl* heightSensorController1;
    HeightSensorControl* heightSensorController2;
    FSMController* fsmController;
    Mock_Decoder* decoder1;
    Mock_Decoder* decoder2;
    Mock_Actuators_Wrapper* actuatorsWrapper1;
    Mock_Actuators_Wrapper* actuatorsWrapper2;

    std::chrono::milliseconds pulseMargin{50};
};

// TEST_F(SystemTestTwoFesto, eStopAfterServiceModeOnFESTO1) {
// 	decoder1->sendPulse(PULSE_ESTOP_HIGH, 0);
// 	decoder2->sendPulse(PULSE_ESTOP_HIGH, 1);
    
//     decoder1->sendPulse(PULSE_ESTOP_LOW, 0);
//     WAIT(500);

//     // expecting to be in System.EStop 
//     assertActuatorState(
//         FESTO1, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_ON,
//         RedLightState::LIGHT_STATE_ON
//     );
//     assertActuatorState(
//         FESTO2, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_ON,
//         RedLightState::LIGHT_STATE_ON
//     );
// }

// TEST_F(SystemTestTwoFesto, motorStartOnFESTO1WhenLBFInterrupted) {
//     decoder1->sendPulse(PULSE_ESTOP_HIGH, 0);
//     WAIT(500);
//     decoder2->sendPulse(PULSE_ESTOP_HIGH, 1);
//     WAIT(500);
//     decoder1->sendPulse(PULSE_BGS_LONG,0);
//     WAIT(500);
//     decoder1->sendPulse(PULSE_BRS_SHORT, 0);
//     WAIT(500);
//     decoder1->sendPulse(PULSE_BGS_SHORT, 0);
//     WAIT(500);
// 	// mockInitializationEvents();
    
//     // FESTO1
//     EXPECT_EQ(actuatorController1->getGreenBlinking(),false);
//     EXPECT_EQ(actuatorController1->getYellowBlinking(),false);
//     EXPECT_EQ(actuatorController1->getRedBlinking(),false);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(LG_PIN),1);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(LY_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(LR_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_FORWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_SLOW_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_BACKWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_STOP_PIN),1);

//     // FESTO2
//     EXPECT_EQ(actuatorController2->getGreenBlinking(),false);
//     EXPECT_EQ(actuatorController2->getYellowBlinking(),false);
//     EXPECT_EQ(actuatorController2->getRedBlinking(),false);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(LG_PIN),1);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(LY_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(LR_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_FORWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_SLOW_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_BACKWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_STOP_PIN),1);
//     WAIT(500);
// 	decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 0);
// 	WAIT(500);
// 	decoder1->sendPulse(PULSE_LBF_OPEN, 0); // TODO -> FSM CONTROLLER SENDS YELLOW LAMP OFF AFTER SI SENDS YELLOW BLINKING????????????
//     WAIT(2000);
//     // FESTO1
//     EXPECT_EQ(actuatorController1->getGreenBlinking(),false);
//     EXPECT_EQ(actuatorController1->getYellowBlinking(),true);
//     EXPECT_EQ(actuatorController1->getRedBlinking(),false);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(LG_PIN),1);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(LY_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(LR_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_FORWARD_PIN),1);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_SLOW_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_BACKWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper1->getActuators(M_STOP_PIN),0);

//     // FESTO2
//     EXPECT_EQ(actuatorController2->getGreenBlinking(),false);
//     EXPECT_EQ(actuatorController2->getYellowBlinking(),true);
//     EXPECT_EQ(actuatorController2->getRedBlinking(),false);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(LG_PIN),1);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(LY_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(LR_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_FORWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_SLOW_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_BACKWARD_PIN),0);
//     EXPECT_EQ(actuatorsWrapper2->getActuators(M_STOP_PIN),1);

// }

// TEST_F(SystemTestTwoFesto, validProfile) {
//     mockInitializationEvents();

//     // LBF1
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBF_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBF_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     // HS1
//     WAIT(Duration::HeightSensor::Mean::Fast + pulseMargin);
//     mockValidProfile(adc1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_SLOW,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     adc1->blockUntilSamplingDone();
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLING_DONE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     // LBM1
//     WAIT(Duration::Sorting::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBM_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBM_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBM_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBE1
//     WAIT(Duration::Egress::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBE_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBE_OPEN, 0);
//     WAIT(pulseMargin);
//     // assertActuatorState(
//     //     "after FST1 LBE_OPEN",
//     //     FESTO1, 
//     //     MotorState::MOTOR_STATE_FAST,
//     //     GreenLightState::LIGHT_STATE_ON, // FIXME LIGHT_STATE_BLINKING (FSM bug?)
//     //     YellowLightState::LIGHT_STATE_OFF,
//     //     RedLightState::LIGHT_STATE_OFF
//     // );
//     assertActuatorState(
//         "after FST1 LBE_OPEN",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     return;

//     // LBF2
//     WAIT(Duration::Ingress::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 1);
//     decoder1->sendPulse(PULSE_LBF_OPEN, 1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 LBF_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // HS2
//     WAIT(Duration::HeightSensor::Mean::Fast - pulseMargin);
//     mockValidProfile(adc2);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 HS_SAMPLE",
//         FESTO2, 
//         MotorState::MOTOR_STATE_SLOW,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     adc2->blockUntilSamplingDone();
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 HS_SAMPLING_DONE",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBM2
//     WAIT(Duration::Sorting::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBM_INTERRUPTED, 1);
//     decoder1->sendPulse(PULSE_LBM_OPEN, 1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 LBM_OPEN",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBE2
//     WAIT(Duration::Egress::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBE_INTERRUPTED, 1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 LBE_INTERRUPTED",
//         FESTO2, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     decoder1->sendPulse(PULSE_LBE_OPEN, 1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 LBE_OPEN",
//         FESTO2, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
// }

// // FIXME Failing Test, motor not stopping on ejection
// TEST_F(SystemTestTwoFesto, invalidProfile) {
//     mockInitializationEvents();
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBF_OPEN, 0);

//     WAIT(Duration::HeightSensor::Mean::Fast);
//     mockInValidProfile(adc1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_SLOW,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

    
//     adc1->blockUntilSamplingDone();
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLING_DONE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     WAIT(Duration::Sorting::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBM_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBM_OPEN, 0);
//     decoder1->sendPulse(PULSE_LBR_INTERRUPTED, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBR_INTERRUPTED",
//         FESTO1, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_ON,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     decoder1->sendPulse(PULSE_LBR_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBR_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
// }

// TEST_F(SystemTestTwoFesto, mismatch1Profile) {
//     mockInitializationEvents();

//     // LBF1
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBF_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBF_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     // HS1
//     WAIT(Duration::HeightSensor::Mean::Fast - pulseMargin);
//     mockValidProfile(adc1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_SLOW,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     adc1->blockUntilSamplingDone();
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLING_DONE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     // LBM1
//     WAIT(Duration::Sorting::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBM_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBM_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBM_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBE1
//     WAIT(Duration::Egress::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBE_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBE_OPEN, 0);
//     WAIT(pulseMargin);
//     // assertActuatorState(
//     //     "after FST1 LBE_OPEN",
//     //     FESTO1, 
//     //     MotorState::MOTOR_STATE_FAST,
//     //     GreenLightState::LIGHT_STATE_ON, // FIXME LIGHT_STATE_BLINKING (FSM bug?)
//     //     YellowLightState::LIGHT_STATE_OFF,
//     //     RedLightState::LIGHT_STATE_OFF
//     // );
//     assertActuatorState(
//         "after FST1 LBE_OPEN",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBF2
//     WAIT(Duration::Ingress::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 1);
//     WAIT(pulseMargin);
//     // assertActuatorState(
//     //     "after FST2 LBF_INTERRUPTED",
//     //     FESTO1, 
//     //     MotorState::MOTOR_STATE_FAST,
//     //     GreenLightState::LIGHT_STATE_ON, // FIXME LIGHT_STATE_BLINKING (FSM bug?)
//     //     YellowLightState::LIGHT_STATE_OFF,
//     //     RedLightState::LIGHT_STATE_OFF
//     // );
//     decoder1->sendPulse(PULSE_LBF_OPEN, 1);
//     WAIT(pulseMargin);
//     // assertActuatorState(
//     //     "after FST2 LBF_OPEN",
//     //     FESTO1, 
//     //     MotorState::MOTOR_STATE_OFF,
//     //     GreenLightState::LIGHT_STATE_ON, // FIXME LIGHT_STATE_BLINKING (FSM bug?)
//     //     YellowLightState::LIGHT_STATE_OFF,
//     //     RedLightState::LIGHT_STATE_OFF
//     // );

//     // HS2
//     WAIT(Duration::HeightSensor::Mean::Fast - pulseMargin);
//     mockInValidProfile(adc2);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 HS_SAMPLE",
//         FESTO2, 
//         MotorState::MOTOR_STATE_SLOW,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     adc2->blockUntilSamplingDone();
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 HS_SAMPLING_DONE",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBM2
//     WAIT(Duration::Sorting::Mean::Fast - pulseMargin);
//     decoder1->sendPulse(PULSE_LBM_INTERRUPTED, 1);
//     decoder1->sendPulse(PULSE_LBM_OPEN, 1);
//     decoder1->sendPulse(PULSE_LBR_INTERRUPTED, 1);;
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 LBR_INTERRUPTED",
//         FESTO2, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_ON,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     decoder1->sendPulse(PULSE_LBR_OPEN, 1);;
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST2 LBR_OPEN",
//         FESTO2, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
// }

// TEST_F(SystemTestTwoFesto, pauseBothFestos) {
//     mockInitializationEvents();

//     // LBF1
//     Logger::getInstance().log(LogLevel::INFO, "[Puk1] entering LBF_1", "SystemTestTwoFesto.pauseBothFestos");
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBF_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBF_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     // HS1
//     WAIT(Duration::HeightSensor::Mean::Fast - pulseMargin);
//     Logger::getInstance().log(LogLevel::INFO, "[Puk1] entering HS_1", "SystemTestTwoFesto.pauseBothFestos");
//     mockValidProfile(adc1);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_SLOW,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     adc1->blockUntilSamplingDone();
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 HS_SAMPLING_DONE",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
    
//     // LBM1
//     WAIT(Duration::Sorting::Mean::Fast - pulseMargin);
//     Logger::getInstance().log(LogLevel::INFO, "[Puk1] entering LBM_1", "SystemTestTwoFesto.pauseBothFestos");
//     decoder1->sendPulse(PULSE_LBM_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBM_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBM_OPEN",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBE1
//     WAIT(Duration::Egress::Mean::Fast - pulseMargin);
//     Logger::getInstance().log(LogLevel::INFO, "[Puk1] entering LBE_1", "SystemTestTwoFesto.pauseBothFestos");
//     decoder1->sendPulse(PULSE_LBE_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBE_OPEN, 0);
//     WAIT(pulseMargin);
//     // assertActuatorState(
//     //     "after FST1 LBE_OPEN",
//     //     FESTO1, 
//     //     MotorState::MOTOR_STATE_FAST,
//     //     GreenLightState::LIGHT_STATE_ON, // FIXME LIGHT_STATE_BLINKING (FSM bug?)
//     //     YellowLightState::LIGHT_STATE_OFF,
//     //     RedLightState::LIGHT_STATE_OFF
//     // );
//     assertActuatorState(
//         "after FST1 LBE_OPEN",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // LBF2
//     WAIT(Duration::Ingress::Mean::Fast - pulseMargin);
//     Logger::getInstance().log(LogLevel::INFO, "[Puk1] entering LBF_2", "SystemTestTwoFesto.pauseBothFestos");
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 1);
//     WAIT(pulseMargin);
//     // assertActuatorState(
//     //     "after FST2 LBF_INTERRUPTED",
//     //     FESTO1, 
//     //     MotorState::MOTOR_STATE_FAST,
//     //     GreenLightState::LIGHT_STATE_ON, // FIXME LIGHT_STATE_BLINKING (FSM bug?)
//     //     YellowLightState::LIGHT_STATE_OFF,
//     //     RedLightState::LIGHT_STATE_OFF
//     // );
//     decoder1->sendPulse(PULSE_LBF_OPEN, 1);
//     WAIT(pulseMargin);
//     // assertActuatorState(
//     //     "after FST2 LBF_OPEN",
//     //     FESTO1, 
//     //     MotorState::MOTOR_STATE_OFF,
//     //     GreenLightState::LIGHT_STATE_ON, // FIXME LIGHT_STATE_BLINKING (FSM bug?)
//     //     YellowLightState::LIGHT_STATE_OFF,
//     //     RedLightState::LIGHT_STATE_OFF
//     // );

//     // LBF1 2nd puk
//     Logger::getInstance().log(LogLevel::INFO, "[Puk2] entering LBF_1", "SystemTestTwoFesto.pauseBothFestos");
//     decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 0);
//     decoder1->sendPulse(PULSE_LBF_OPEN, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 LBF_OPEN (Puk2)",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     assertActuatorState(
//         "after FST1 LBF_OPEN (Puk2)",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // pause
//     decoder1->sendPulse(PULSE_BRS_SHORT, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 BRS_SHORT",
//         FESTO1, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_OFF,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     assertActuatorState(
//         "after FST1 BRS_SHORT",
//         FESTO2, 
//         MotorState::MOTOR_STATE_OFF,
//         GreenLightState::LIGHT_STATE_OFF,
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // continue
//     WAIT(pulseMargin);
//     Logger::getInstance().log(LogLevel::INFO, "Continue", "SystemTestTwoFesto.pauseBothFestos");
//     decoder1->sendPulse(PULSE_BGS_SHORT, 0);
//     WAIT(pulseMargin);
//     assertActuatorState(
//         "after FST1 BGS_SHORT",
//         FESTO1, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_ON,  // FIXME LIGHT_STATE_OFF (FSM bug?)
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );
//     assertActuatorState(
//         "after FST1 BGS_SHORT",
//         FESTO2, 
//         MotorState::MOTOR_STATE_FAST,
//         GreenLightState::LIGHT_STATE_BLINKING,  // FIXME LIGHT_STATE_OFF (FSM bug?)
//         YellowLightState::LIGHT_STATE_OFF,
//         RedLightState::LIGHT_STATE_OFF
//     );

//     // FIXME C++ exception with description "thread::join failed: Resource busy" thrown in TearDown(). (blinking thread bug?)
// }