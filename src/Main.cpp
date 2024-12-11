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
#include "LogikParallel/FSM_System_Parallel.h"
#include "LogikParallel/FSM_Ingress_Parallel.h"
#include "LogikParallel/FSM_HeightMeasurement_Parallel.h"
#include "LogikParallel/FSM_Sorting_Parallel.h"
#include "LogikParallel/FSM_Egress_Parallel.h"
#include "LogikParallel/FSM_Motor_Parallel.h"


int main() {
    std::string dispatcherChannelName = "dispatcher";
    std::string actuatorControllerChannelName = "actuatorController";

    Dispatcher *dispatcher = new Dispatcher(dispatcherChannelName);
    Decoder *decoder = new Decoder(dispatcherChannelName);
    Actuators_Wrapper *actuatorsWrapper = new Actuators_Wrapper();
    ActuatorController *actuatorController = new ActuatorController(actuatorControllerChannelName, actuatorsWrapper);
    HeightSensorControl *heightSensorController = new HeightSensorControl("HSControl", dispatcherChannelName);
    FSM_System_Parallel *fsm_System_Parallel = new FSM_System_Parallel(dispatcherChannelName);
    FSM_Ingress_Parallel *fsm_Ingress_Parallel = new FSM_Ingress_Parallel(dispatcherChannelName);
    FSM_HeightMeasurement_Parallel *fsm_HeightMeasurement_Parallel = new FSM_HeightMeasurement_Parallel(dispatcherChannelName);
    FSM_Sorting_Parallel *fsm_Sorting_Parallel = new FSM_Sorting_Parallel(dispatcherChannelName);
    FSM_Egress_Parallel *fsm_Egress_Parallel = new FSM_Egress_Parallel(dispatcherChannelName);
    FSM_Motor_Parallel *fsm_Motor_Parallel = new FSM_Motor_Parallel(dispatcherChannelName);



    dispatcher->addSubscriber(
    			actuatorController->getChannel(), actuatorController->getPulses(), actuatorController->getNumOfPulses()
    	);
    dispatcher->addSubscriber(
        		fsm_System_Parallel->getChannel(), fsm_System_Parallel->getPulses(), fsm_System_Parallel->getNumOfPulses()
        );
    dispatcher->addSubscriber(
        		fsm_Ingress_Parallel->getChannel(), fsm_Ingress_Parallel->getPulses(), fsm_Ingress_Parallel->getNumOfPulses()
        );
    dispatcher->addSubscriber(
    			fsm_HeightMeasurement_Parallel->getChannel(), fsm_HeightMeasurement_Parallel->getPulses(), fsm_HeightMeasurement_Parallel->getNumOfPulses()
        );
    dispatcher->addSubscriber(
    			fsm_Sorting_Parallel->getChannel(), fsm_Sorting_Parallel->getPulses(), fsm_Sorting_Parallel->getNumOfPulses()
       );
    dispatcher->addSubscriber(
       			fsm_Egress_Parallel->getChannel(), fsm_Egress_Parallel->getPulses(), fsm_Egress_Parallel->getNumOfPulses()
          );
    dispatcher->addSubscriber(
    			fsm_Motor_Parallel->getChannel(), fsm_Motor_Parallel->getPulses(), fsm_Motor_Parallel->getNumOfPulses()
              );

    std::thread heightSensorControllerThread(std::bind(&HeightSensorControl::handleMsg, heightSensorController));
    std::thread dispatcherThread(std::bind(&Dispatcher::handleMsg, dispatcher));
    std::thread actuatorControllerThread(std::bind(&ActuatorController::handleMsg, actuatorController));
    std::thread decoderThread(std::bind(&Decoder::handleMsg, decoder));
    std::thread fsm_System_ParallelThread(std::bind(&FSM_System_Parallel::handleMsg,fsm_System_Parallel));
    std::thread fsm_Ingress_ParallelThread(std::bind(&FSM_Ingress_Parallel::handleMsg,fsm_Ingress_Parallel));
    std::thread fsm_HeightMeasurement_ParallelThread(std::bind(&FSM_HeightMeasurement_Parallel::handleMsg,fsm_HeightMeasurement_Parallel));
    std::thread fsm_Sorting_ParallelThread(std::bind(&FSM_Sorting_Parallel::handleMsg,fsm_Sorting_Parallel));
    std::thread fsm_Egress_ParallelThread(std::bind(&FSM_Egress_Parallel::handleMsg,fsm_Egress_Parallel));
    std::thread fsm_Motor_ParallelThread(std::bind(&FSM_Motor_Parallel::handleMsg,fsm_Motor_Parallel));


     WAIT(1000);

     int32_t dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_LONG, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_SHORT, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_INTERRUPTED, 0)) {
          perror("ups");
       }
     WAIT(500);
      if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBF_OPEN, 0)) {
           perror("ups");
      }

     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLE, 0)) {
         perror("ups");
     }

     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLING_DONE, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBM_INTERRUPTED, 0)) {
         perror("ups");
     }
    // WAIT(500);
    // if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBM_OPEN, 0)) {
    //     perror("ups");
    // }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBE_INTERRUPTED, 0)) {
         perror("ups");
     }

     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_LBE_OPEN, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_ESTOP_LOW, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_ESTOP_HIGH, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_SHORT, 0)) {
         perror("ups");
     }
     WAIT(500);
     if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, 0)) {
         perror("ups");
     }
     WAIT(500);

    // join threads
    std::cout << "\nThreads, started, main going idle...\n" << std::endl;

    fsm_Motor_ParallelThread.join();
    fsm_Egress_ParallelThread.join();
    fsm_Sorting_ParallelThread.join();
    fsm_HeightMeasurement_ParallelThread.join();
    fsm_Ingress_ParallelThread.join();
    dispatcherThread.join();
    fsm_System_ParallelThread.join();
    actuatorControllerThread.join();
    decoderThread.join();


    return 0;
}
