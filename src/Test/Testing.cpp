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
#include "../HeightController/header/HeightSensorControl.h"
#include "../Decoder/headers/Decoder.h"
#include "../FSM/FSMController.h"
#include "../Logging/headers/Logger.h"
#include "../HeartBeat/headers/HeartBeat.h"
#include "../FSM/gen/src-gen/FSM.h"
#include "../FSM/gen/src/sc_rxcpp.h"


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
        if (!fsmController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop fsmControllerHandleMsgThread has not ended...", "SystemTest");}
        // if (!decoder->stop()) {std::cout << "loop decoderThread has ended" << std::endl;} // MOCK DOESNT HAVE A MSGHANDLER
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
            actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
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
        ADC* adc1 = new ADC(*tsc1);
        ADC* adc2 = new ADC(*tsc2);
        heightSensorController1 = new HeightSensorControl("HSControl1", dispatcherChannelName, FESTO1, tsc1, adc1); 
        heightSensorController2 = new HeightSensorControl("HSControl2", dispatcherChannelName, FESTO2, tsc2, adc2); 


        heightSensorControllerThread1 = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController1));
        heightSensorControllerThread2 = std::thread(std::bind(&HeightSensorControl::handleMsg, heightSensorController2));

        fsmController = new FSMController(dispatcherChannelName);
        dispatcher->addSubscriber(
            fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        );
        WAIT(1000);
        Logger::getInstance().log(LogLevel::DEBUG, "Starting fsmController...", "SystemTestTwoFesto");
        fsmControllerHandleMsgThread = std::thread(std::bind(&FSMController::handleMsg, fsmController));
        Logger::getInstance().log(LogLevel::DEBUG, "Starting DECODER...", "SystemTestTwoFesto");
        decoderThread1 = std::thread(std::bind(&Mock_Decoder::handleMsg, decoder1));
        decoderThread2 = std::thread(std::bind(&Mock_Decoder::handleMsg, decoder2));
        WAIT(1000);
    }

    void TearDown() override {
        Logger::getInstance().log(LogLevel::INFO, "Tear Down Tests...", "SystemTestTwoFesto");
        if (!fsmController->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop fsmControllerHandleMsgThread has not ended...", "SystemTestTwoFesto");}
        // if (!decoder->stop()) {std::cout << "loop decoderThread has ended" << std::endl;} // MOCK DOESNT HAVE A MSGHANDLER
        if (!actuatorController1->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop actuatorControllerThread1 has not ended...", "SystemTestTwoFesto");}
        if (!actuatorController2->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop actuatorControllerThread1 has not ended...", "SystemTestTwoFesto");}
        if (!heightSensorController1->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop heightSensorControllerThread1 has not ended...", "SystemTestTwoFesto");}
        if (!heightSensorController2->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop heightSensorControllerThread2 has not ended...", "SystemTestTwoFesto");}
        if (!dispatcher->stop()) {Logger::getInstance().log(LogLevel::INFO, "loop dispatcherThread has not ended...", "SystemTestTwoFesto");}

        Logger::getInstance().log(LogLevel::INFO, "ENDING TREADS...", "SystemTestTwoFesto");
        // if (fsmControllerHandleMsgThread.joinable()) 
        fsmControllerHandleMsgThread.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread fsmControllerHandleMsgThread Ended...", "SystemTestTwoFesto");
        // if (decoderThread.joinable()) 
        decoderThread1.join();
        decoderThread2.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread decoderThread Ended...", "SystemTestTwoFesto");
        // if (actuatorControllerThread.joinable()) 
        actuatorControllerThread1.join();
        actuatorControllerThread2.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread actuatorControllerThread Ended...", "SystemTestTwoFesto");
        // if (heightSensorControllerThread.joinable()) 
        heightSensorControllerThread1.join();  
        heightSensorControllerThread2.join();  
        Logger::getInstance().log(LogLevel::INFO, "Thread heightSensorControllerThread Ended...", "SystemTestTwoFesto"); 
        // if (dispatcherThread.joinable()) 
        dispatcherThread.join();
        Logger::getInstance().log(LogLevel::INFO, "Thread dispatcherThread Ended...", "SystemTestTwoFesto");

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
    HeightSensorControl* heightSensorController1;
    HeightSensorControl* heightSensorController2;
    FSMController* fsmController;
    Mock_Decoder* decoder1;
    Mock_Decoder* decoder2;
    Mock_Actuators_Wrapper* actuatorsWrapper1;
    Mock_Actuators_Wrapper* actuatorsWrapper2;


};


TEST_F(SystemTestTwoFesto, eStopAfterServiceModeOnFESTO1) {
	decoder1->sendPulse(PULSE_ESTOP_HIGH, 0);
	decoder2->sendPulse(PULSE_ESTOP_HIGH, 1);
    
    decoder1->sendPulse(PULSE_ESTOP_LOW, 0);
    WAIT(500);
    // FESTO1
    EXPECT_EQ(actuatorController1->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController1->getYellowBlinking(),false);
    EXPECT_EQ(actuatorController1->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper1->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper1->getActuators(LY_PIN),1);
    EXPECT_EQ(actuatorsWrapper1->getActuators(LR_PIN),1);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_FORWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_STOP_PIN),1);

    // FESTO2
    EXPECT_EQ(actuatorController2->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController2->getYellowBlinking(),false);
    EXPECT_EQ(actuatorController2->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper2->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper2->getActuators(LY_PIN),1);
    EXPECT_EQ(actuatorsWrapper2->getActuators(LR_PIN),1);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_FORWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_STOP_PIN),1);
    
}

TEST_F(SystemTestTwoFesto, motorStartOnFESTO1WhenLBFInterrupted) {
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
    WAIT(500);
	decoder1->sendPulse(PULSE_LBF_INTERRUPTED, 0);
	WAIT(500);
	decoder1->sendPulse(PULSE_LBF_OPEN, 0); // TODO -> FSM CONTROLLER SENDS YELLOW LAMP OFF AFTER SI SENDS YELLOW BLINKING????????????
    WAIT(2000);
    // FESTO1
    EXPECT_EQ(actuatorController1->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController1->getYellowBlinking(),true);
    EXPECT_EQ(actuatorController1->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper1->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper1->getActuators(LY_PIN),0);
    EXPECT_EQ(actuatorsWrapper1->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_FORWARD_PIN),1);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper1->getActuators(M_STOP_PIN),0);

    // FESTO2
    EXPECT_EQ(actuatorController2->getGreenBlinking(),false);
    EXPECT_EQ(actuatorController2->getYellowBlinking(),true);
    EXPECT_EQ(actuatorController2->getRedBlinking(),false);
    EXPECT_EQ(actuatorsWrapper2->getActuators(LG_PIN),1);
    EXPECT_EQ(actuatorsWrapper2->getActuators(LY_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(LR_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_FORWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_SLOW_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_BACKWARD_PIN),0);
    EXPECT_EQ(actuatorsWrapper2->getActuators(M_STOP_PIN),1);

}
