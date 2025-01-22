#include <functional> // For std::bind
#include <iostream>
#include <string>
#include <thread>

#include "HAL/headers/Actuators_Wrapper.h"
#include "Actuatorcontroller/headers/ActuatorController.h"
#include "Dispatcher/headers/Dispatcher.h"
//#include "Logik/headers/FSM.h"
#include "Util/headers/Util.h"
#include "HAL/headers/ADC.h"
#include "HAL/headers/TSCADC.h"
#include "HeightController/headers/HeightSensorControl.h"
#include "Decoder/headers/Decoder.h"
//Here is the FSM QualityGate and new FSM Controller
#include "FSM/headers/FsmController.h"
// #include "FSM/src-gen/FSM_QualityGate.h"
#include "Logging/headers/Logger.h"
#include "HeartBeat/headers/HeartBeat.h"

#include <gtest/gtest.h>

#define TESTING 0
#define LOGLEVEL TRACE
#define HB 1


int main(int argc, char **argv) {
auto ret = 0;

Logger& logger = Logger::getInstance();


logger.setLogLevel(LogLevel::LOGLEVEL);    // Log-Level setzen
// Log-Level für verschiedene Klassen individuell festlegen
// logger.setLogLevelForClass("Mock_ADC", LogLevel::DEBUG); // Standardmodus
// logger.setLogLevelForClass("ClassB", LogLevel::ERROR, true); // Nur ERROR loggen

//logger.setLogFile("tmp/loggingFile.log");   // Log-Datei festlegen INFO: FILE IS ON TARGET

logger.start();                        // Logger starten
logger.log(LogLevel::INFO, "Application starting...", "Main");

#if TESTING == 1

    logger.log(LogLevel::INFO, "Testing starting...", "Main");
	::testing::InitGoogleTest(&argc, argv);
	ret = RUN_ALL_TESTS();

#else

    logger.log(LogLevel::INFO, "GNS must Be Running...", "Main");




    if (argc < 2) {
        if (argc <= 1) {
            logger.log(LogLevel::ERROR, "No argument given...", "Main");
        } else {
            logger.log(LogLevel::ERROR, "Too few arguments given...", "Main");
        }
        logger.log(LogLevel::INFO, std::string("Usage: ") + argv[0] + " -s | -c", "Main");
        ret = EXIT_FAILURE;
    } else if (std::strcmp(argv[1], "-s") == 0) {
        logger.log(LogLevel::INFO, "Running as Server -> FESTO1...", "Main");
        system("gns -s");
        
               
        std::string dispatcherChannelName = "dispatcher";
        Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);
        std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));

          
        
        Decoder *decoder = new Decoder(dispatcherChannelName, FESTO1);
        FSMController *fsmController = new FSMController(dispatcherChannelName);
        dispatcher->addSubscriber(
            fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        );
        std::string actuatorControllerChannelName = "actuatorController1";
        Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();

        ActuatorController *actuatorController = new ActuatorController(FESTO1, actuatorControllerChannelName, actuatorsWrapper);
         std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
        actuatorController->subscribeToDispatcher();
#if HB == 1
        HeartBeat* hb1 = new HeartBeat(FESTO1, actuatorController->getHBChannel());

        std::thread hb1SendThread(std::bind(&HeartBeat::sendMsg, hb1));
        WAIT(200);
        std::thread hb1ReicvThread(std::bind(&HeartBeat::handleMsg, hb1));
        std::thread hb1DisconnectThread(std::bind(&HeartBeat::checkConnection, hb1));  
#endif
        TSCADC* tsc = new TSCADC();
        ADC* adc = new ADC(*tsc);
        HeightSensorControl *heightSensorController = new HeightSensorControl("HSControl1", dispatcherChannelName, FESTO1, tsc, adc);
        std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));

        

        
        std::thread fsmControllerHandleMsgThread(std::bind(&FSMController::handleMsg, fsmController));
       
        std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));


        

        dispatcherThread.join();
        
        fsmControllerHandleMsgThread.join();

        actuatorControllerThread.join();
        decoderThread.join();
        heightSensorControllerThread.join();


    } else if (std::strcmp(argv[1], "-c") == 0) {
        logger.log(LogLevel::INFO, "Running as Client -> FESTO2...", "Main");
        system("gns -c");
        
        
        std::string dispatcherChannelName = "dispatcher";
        
        Decoder *decoder = new Decoder(dispatcherChannelName, FESTO2);
        logger.log(LogLevel::DEBUG, "Decoder created", "Main");
        std::string actuatorControllerChannelName = "actuatorController2";
      
        Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
   
        ActuatorController *actuatorController = new ActuatorController(FESTO2,actuatorControllerChannelName, actuatorsWrapper);
        logger.log(LogLevel::DEBUG, "ActuatorController created", "Main");
        std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
        std::thread actuatorControllerLocalThread(std::bind(&ActuatorController::handleHbMsg, actuatorController));

        actuatorController->subscribeToDispatcher();
#if HB == 1
        HeartBeat* hb2 = new HeartBeat(FESTO2, actuatorController->getHBChannel());

        std::thread hb2SendThread(std::bind(&HeartBeat::sendMsg, hb2));
        WAIT(200);
        std::thread hb2ReicvThread(std::bind(&HeartBeat::handleMsg, hb2));
        std::thread hb2DisconnectThread(std::bind(&HeartBeat::checkConnection, hb2));
#endif   
        TSCADC* tsc = new TSCADC();
        ADC* adc = new ADC(*tsc);
        // logger.log(LogLevel::DEBUG, "ActuatorController init done", "Main");
        HeightSensorControl *heightSensorController = new HeightSensorControl("HSControl2", dispatcherChannelName, FESTO2, tsc, adc);

        



        std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));
        
        std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));


        heightSensorControllerThread.join();
        actuatorControllerThread.join();
        decoderThread.join();




    



    } else {
        logger.log(LogLevel::INFO, "Running Fake Setup as Server ...", "Main");
        logger.log(LogLevel::INFO, std::string("Usage: ") + argv[0] + " -s | -c", "Main");
        
    }

#endif /*_TESTING_*/
    logger.log(LogLevel::CRITICAL, "ENDING PROGRAM...", "Main");
    logger.stop();
    return ret;
}

