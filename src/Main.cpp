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
        // TODO run as Server TO BE TESTED
               
        std::string dispatcherChannelName = "dispatcher";
        Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);
        std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));

          
        
        Decoder *decoder = new Decoder(dispatcherChannelName, FESTO1);
        std::string actuatorControllerChannelName = "actuatorController1";
        Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
        ActuatorController *actuatorController = new ActuatorController(FESTO1, actuatorControllerChannelName, actuatorsWrapper);
         std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
        actuatorController->subscribeToDispatcher();

        HeartBeat* hb1 = new HeartBeat(FESTO1, actuatorController->getChannel());
        hb1->connectToFesto();
        std::thread hb1SendThread(std::bind(&HeartBeat::sendMsg, hb1));
        std::thread hb1ReicvThread(std::bind(&HeartBeat::handleMsg, hb1));

        TSCADC* tsc = new TSCADC();
        ADC* adc = new ADC(*tsc);
        HeightSensorControl *heightSensorController = new HeightSensorControl("HSControl1", dispatcherChannelName, FESTO1, tsc, adc);
        std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));
        FSMController *fsmController = new FSMController(dispatcherChannelName);
        dispatcher->addSubscriber(
            fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        );

        

        
        std::thread fsmControllerHandleMsgThread(std::bind(&FSMController::handleMsg, fsmController));
       
        std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));


        

        dispatcherThread.join();
        // fsmThread.join();
        fsmControllerHandleMsgThread.join();
        //fsmControllerSendMsgThread.join();
        actuatorControllerThread.join();
        decoderThread.join();



    } else if (std::strcmp(argv[1], "-c") == 0) {
        logger.log(LogLevel::INFO, "Running as Client -> FESTO2...", "Main");
        system("gns");
        // TODO run as Client TO BE TESTED
        
        std::string dispatcherChannelName = "dispatcher";
        
        Decoder *decoder = new Decoder(dispatcherChannelName, FESTO2);
        logger.log(LogLevel::DEBUG, "Decoder created", "Main");
        std::string actuatorControllerChannelName = "actuatorController2";
      
        Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
   
        ActuatorController *actuatorController = new ActuatorController(FESTO2,actuatorControllerChannelName, actuatorsWrapper);
        logger.log(LogLevel::DEBUG, "ActuatorController created", "Main");
        std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
        actuatorController->subscribeToDispatcher();

        HeartBeat* hb2 = new HeartBeat(FESTO2, actuatorController->getChannel());
        hb2->connectToFesto();
        std::thread hb2SendThread(std::bind(&HeartBeat::sendMsg, hb2));
        std::thread hb2ReicvThread(std::bind(&HeartBeat::handleMsg, hb2));
        
        TSCADC* tsc = new TSCADC();
        ADC* adc = new ADC(*tsc);
        // logger.log(LogLevel::DEBUG, "ActuatorController init done", "Main");
        HeightSensorControl *heightSensorController = new HeightSensorControl("HSControl2", dispatcherChannelName, FESTO2, tsc, adc);

        



        std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));
        
        std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));


        hb2->connectToFesto();
        std::thread hbSendThread(std::bind(&HeartBeat::sendMsg, hb2));
        std::thread hbReicvThread(std::bind(&HeartBeat::handleMsg, hb2));

        heightSensorControllerThread.join();
        actuatorControllerThread.join();
        decoderThread.join();




        // TODO wenn Client viel später als der Server an ist, also die fsm schon im wartezusatnd bekommet der client die actuator-
        // Befehler für die lampe und motor aus nicht mit. also muss das wahrscheinlich erst passieren wenn der erste 
        // e-stop check durch ist!







    } else {
        logger.log(LogLevel::INFO, "Running Fake Setup as Server ...", "Main");
        logger.log(LogLevel::INFO, std::string("Usage: ") + argv[0] + " -s | -c", "Main");
        

        // std::string dispatcherChannelName = "dispatcher";
        // std::cout << "1" << std::endl;
        // Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);
        // // dispatcher->addSubSbrber(int32_t coid, uint_8[])
        // std::cout << "2" << std::endl;
        // Decoder *decoder = new Decoder(dispatcherChannelName, FESTO1);

        // std::string actuatorControllerChannelName = "actuatorController";
        // Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
        // ActuatorController *actuatorController = new ActuatorController(FESTO1,actuatorControllerChannelName, actuatorsWrapper);
        // //dispatcher->addSubscriber(
        // //    actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
        // //);
        // std::cout << "3" << std::endl;
        // //init HS
        // TSCADC* tsc = new TSCADC();
        // ADC* adc = new ADC(*tsc);
        // HeightSensorControl *heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName, FESTO1, tsc, adc); // Create an object of HwAdcDemo
        // //heightSensorController->initRoutine();
        // //start Thread
        // std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));

        // // std::string fsmChannelName = "fsm";
        // // FSM *fsm = new FSM(fsmChannelName);
        // // fsm->connectToChannel(dispatcher->getChannel());
        // // dispatcher->addSubscriber(
        // //     fsm->getChannel(), fsm->getPulses(), fsm->getNumOfPulses()
        // // );

        // FSMController *fsmController = new FSMController(dispatcherChannelName);
        // std::cout << "3.5" << std::endl;
        // dispatcher->addSubscriber(
        //     fsmController->getChannel(), fsmController->getPulses(), fsmController->getNumOfPulses()
        // );
        // std::cout << "4" << std::endl;

        // // start threads
        // std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));
        // WAIT(1000);
        // // std::thread fsmThread(std::bind(&FSM::handleMsg, fsm));
        // std::thread fsmControllerHandleMsgThread(std::bind(&FSMController::handleMsg, fsmController));
        // //std::thread fsmControllerSendMsgThread(std::bind(&FSMController::sendMsg, fsmController));
        // std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
        // std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));
        // std::cout << "1" << std::endl;

        // WAIT(1000);
        // //int32_t dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
        // //if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_SM1_RESTING, 0)) {
        // //    perror("Event onPukPresentIn Failed\n");
        // //}

        // // WAIT(1000);
        // // int32_t dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_LONG, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_SHORT, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_INTERRUPTED, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_OPEN, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLE, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLING_DONE, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBM_INTERRUPTED, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBM_OPEN, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBE_INTERRUPTED, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_INTERRUPTED, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBE_OPEN, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_ESTOP_LOW, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_ESTOP_HIGH, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_SHORT, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);
        // // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, 0)) {
        // //     perror("ups");
        // // }
        // // WAIT(500);

        // std::cout << "5" << std::endl;
        // // join threads
        // std::cout << "\nThreads, started, main going idle...\n" << std::endl;


        // dispatcherThread.join();
        // // fsmThread.join();
        // fsmControllerHandleMsgThread.join();
        // //fsmControllerSendMsgThread.join();
        // actuatorControllerThread.join();
        // decoderThread.join();
    }

#endif /*_TESTING_*/
    logger.log(LogLevel::CRITICAL, "ENDING PROGRAM...", "Main");
    logger.stop();
    return ret;
}

