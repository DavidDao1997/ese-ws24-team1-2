/*
 * FSMController.cpp
 *
 *  Created on: 07.12.2024
 *      Author: Marc
 */

#include "headers/FSMController.h"

int8_t FSMController::numOfPulses = FSM_CONTROLLER_NUM_OF_PULSES;
int8_t FSMController::pulses[FSM_CONTROLLER_NUM_OF_PULSES] = {
    PULSE_ESTOP_HIGH,
    PULSE_ESTOP_LOW,
    PULSE_LBF_INTERRUPTED,
    PULSE_LBF_OPEN,
    PULSE_LBE_INTERRUPTED,
    PULSE_LBE_OPEN,
    PULSE_LBR_INTERRUPTED,
    PULSE_LBR_OPEN,
    PULSE_LBM_INTERRUPTED,
    PULSE_LBM_OPEN,
    PULSE_BGS_SHORT,
    PULSE_BGS_LONG,
    PULSE_BRS_SHORT,
    PULSE_BGR_SHORT,
    PULSE_HS_SAMPLE,
    PULSE_FSM,
    PULSE_HS_SAMPLING_DONE,
    PULSE_MS_TRUE,
    PULSE_MS_FALSE
};

#define FESTO1 0
#define FESTO2 1

FSMController::FSMController(const std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;
    subThreadsRunning = false;

    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);

    // FSM_QualityGate qualityGateInstance;
    fsm = new FSM_QualityGate();

    // callback for FSM System
    fsmSystem = new FSMSystem(dispatcherConnectionID);

    fsmSystem->onSystemServiceIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SYSTEM_SERVICE_IN)) {
            perror("Service In Failed\n");
        }
    });
    fsmSystem->onSystemServiceOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SYSTEM_SERVICE_OUT)) {
            perror("Service Out Failed\n");
        }
    });
    fsmSystem->onEStopIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SYSTEM_ESTOP_IN)) {
            perror("ESTOP In Failed\n");
        }
    });
    fsmSystem->onEStopOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SYSTEM_ESTOP_OUT)) {
            perror("ESTOP Out Failed\n");
        }
    });
    fsmSystem->onSystemOperationalIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SYSTEM_OPERATIONAL_IN)) {
            perror("Operational In Failed\n");
        }
    });
    fsmSystem->onSystemOperationalOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SYSTEM_OPERATIONAL_OUT)) {
            perror("Operational Out Failed\n");
        }
    });

    // callback for LED Green
    fsmLG1 = new FSMLampGreen(dispatcherConnectionID);
    fsmLG1->onLG1On([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LG1_ON, 0)) {
            perror("LG1 On Failed\n");
        }
    });
    fsmLG1->onLG1Off([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LG1_OFF, 0)) {
            perror("LG1 Off failed\n");
        }
    });
    fsmLG1->onLG1Blinking1Hz([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LG1_BLINKING, 1000)) {
            perror("LY1 Blink Failed\n");
        }
    });

    // callback for LED Yellow
    fsmLY1 = new FSMLampYellow(dispatcherConnectionID);

    fsmLY1->onLY1On([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LY1_ON, 0)) {
            perror("LY1 On Failed\n");
        }
    });
    fsmLY1->onLY1Off([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LY1_OFF, 0)) {
            perror("LY1 Off failed\n");
        }
    });
    fsmLY1->onLY1Blinking1Hz([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LY1_BLINKING, 1000)) {
            perror("LY1 Blink Failed\n");
        }
    });

    // callback for LED Red
    fsmLR1 = new FSMLampRed(dispatcherConnectionID);

    fsmLR1->onLR1On([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LR1_ON, 0)) {
            perror("LR1 On Failed\n");
        }
    });

    fsmLR1->onLR1Off([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_LR1_OFF, 0)) {
            perror("LR1 Off Failed\n");
        }
    });

    // callback for motor
    fsmMotor = new FSMMotor(dispatcherConnectionID);

    fsmMotor->onMotorStopIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_MOTOR1_STOP, 0)) {
            perror("Event onMotorStopIn Failed\n");
        }
    });
    fsmMotor->onMotorSlowIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_MOTOR1_SLOW, 0)) {
            perror("Event onMotorSlowIn Failed\n");
        }
    });
    fsmMotor->onMotorFastIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_MOTOR1_FAST, 0)) {
            perror("Event onMotorFastIn Failed\n");
        }
    });

    // callback for Ingress
    fsmIngress = new FSMIngress(dispatcherConnectionID);
    fsmIngress->onIngressIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_INGRESS_IN)) {
            perror("Event Ingress in  Failed\n");
        }
    });
    fsmIngress->onIngressOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_INGRESS_OUT)) {
            perror("Event Ingress out  Failed\n");
        }
    });
    fsmIngress->onPukPresentIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_INGRESS_PUKPRESENT_IN)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmIngress->onPukCreatingDistanceOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_INGRESS_CREATINGDISTANCE_OUT)) {
            perror("Event onCreatingDistanceOut Failed\n");
        }
    });
    fsmIngress->onPukEntryHeightMeasurement([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_PUK_ENTRY_HEIGHT_MEASUREMENT)) {
            perror("Event Puk Height Measurement  Failed\n");
        }
    });

    fsmHeightMeasurement = new FSMHeightMeasurement(dispatcherConnectionID);
    fsmHeightMeasurement->onPukPresentIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_HM_PUKPRESENT_IN)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmHeightMeasurement->onPukPresentOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_HM_PUKPRESENT_OUT)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmHeightMeasurement->onMeasurementIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_HM_MEASUREMENT_IN)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmHeightMeasurement->onMeasurementOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_HM_MEASUREMENT_OUT)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmHeightMeasurement->onPukDistanceValid([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_HM_PUK_DISTANCE_VALID)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmHeightMeasurement->onPukLeaveHeightMeasurement([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_HM_PUK_LEAVING_HEIGHTMEASUREMENT)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });

    fsmHeightMeasurement->onPukEntrySorting([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_HM_ENTRY_SORTING)) {
            perror("Event onPukPresentIn Failed \n");
        }
    });

    fsmSorting = new FSMSorting(dispatcherConnectionID);
    fsmSorting->onPukPresentIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_PUKPRESENT_IN)) {
            perror("Event onPuk Present in Failed\n");
        }
    });
    fsmSorting->onPukPresentOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_PUKPRESENT_OUT)) {
            perror("Event Soritng Puk Present out Failed\n");
        }
    });
    fsmSorting->onPukEntryEgress([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_PUK_ENTRY_EGRESS)) {
            perror("Event Sorting Puk Entry Egress Failed\n");
        }
    });
    fsmSorting->onPukEjectorDistanceValid([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_PUK_EJECTOR_DISTANCE_VALID)) {
            perror("Event Sorting Puk Ejector Distance Valid Failed\n");
        }
    });
    fsmSorting->onRampFullIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_RAMP_FULL_IN)) {
            perror("Event Sorting Ramp Full In Failed\n");
        }
    });
    fsmSorting->onRampFullOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_RAMP_FULL_OUT)) {
            perror("Event Sorting Ramp Full Out\n");
        }
    });
    fsmSorting->onMetalMeasurementIn([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_METALMEASUREMENT_IN)) {
            perror("Event Sorting  Failed\n");
        }
    });
    fsmSorting->onMetalMeasurementOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_METALMEASUREMENT_OUT)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmSorting->onSortingModuleActive([](int32_t conId) {
        std::cout << "FSMCONTROLLER: onSortingModuleActive " << std::endl;
        if (0 < MsgSendPulse(conId, -1, PULSE_SM1_ACTIVE, 0)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmSorting->onSortingModuleResting([](int32_t conId) {
        std::cout << "FSMCONTROLLER: onSortingModuleResting " << std::endl;
        if (0 < MsgSendPulse(conId, -1, PULSE_SM1_RESTING, 0)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmSorting->onEjectingOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_EJECTOR_OUT)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });
    fsmSorting->onPassthroughOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_SORTING_PASSTHROUGH_OUT)) {
            perror("Event onPukPresentIn Failed\n");
        }
    });

    // callback for Egress
    fsmEgress = new FSMEgress(dispatcherConnectionID);

    fsmEgress->onEgressTransferIn([](int32_t conId) {
        std::cout << "FSMEgress: entry Transfer " << std::endl;
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_EGRESS_TRANSFER_IN)) {
            perror("Event Egress in  Failed\n");
        }
    });
    fsmEgress->onEgressTransferOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_EGRESS_TRANSFER_OUT)) {
            perror("Event Egress out  Failed\n");
        }
    });
    fsmEgress->onEgressPukPresentIn([](int32_t conId) {
        std::cout << "FSMEgress: entry Transfer " << std::endl;
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_EGRESS_PUKPRESENT_IN)) {
            perror("Event Egress in  Failed\n");
        }
    });
    fsmEgress->onEgressPukPresentOut([](int32_t conId) {
        if (0 < MsgSendPulse(conId, -1, PULSE_FSM, EVENT_EGRESS_PUKPRESENT_OUT)) {
            perror("Event Egress out  Failed\n");
        }
    });
    // TODO connect to dispatcher
    // create a connection to Dispatcher
}

FSMController::~FSMController() {
    running = false;
    subThreadsRunning = false;
    destroyChannel(channelID);
    // TODO disconnect from dispatcher ?? this should disconnect not destroy a channel
    destroyChannel(dispatcherConnectionID);
}

int8_t *FSMController::getPulses() { return pulses; };
int8_t FSMController::getNumOfPulses() { return numOfPulses; };

void FSMController::handleMsg() {
    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);
        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received
            int32_t msgVal = msg.value.sival_int;
            switch (msg.code) {
            case PULSE_ESTOP_HIGH:
                std::cout << "FSMCONTROLLER: received PULSE_ESTOP_HIGH " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSystem->raiseEStop1High();
                } else {
                    // TODO
                }
                break;
            case PULSE_ESTOP_LOW:
                std::cout << "FSMCONTROLLER: received PULSE_ESTOP_LOW " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSystem->raiseEStop1Low();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBF_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBF_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal) {
                    // fsm->raiseLBF_1_INTERRUPTED();
                    fsmIngress->raiseLBF1Interrupted();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBF_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBF_OPEN " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmIngress->raiseLBF1Open();
                    // fsm->raiseLBF_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBE_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBE_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal) {
                    // fsm->raiseLBE_1_INTERRUPTED();
                    fsmEgress->raiseLBE1Interrupted();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBE_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBE_OPEN " << std::endl;
                if (FESTO1 == msgVal) {
                    // fsm->raiseLBE_1_OPEN();
                    fsmEgress->raiseLBE1Open();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBR_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBR_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSorting->raiseLBR1Interrupted();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBR_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBR_OPEN " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSorting->raiseLBR1Open();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBM_INTERRUPTED:
                std::cout << "FSMCONTROLLER: received PULSE_LBM_INTERRUPTED " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSorting->raiseLBM1Interrupted();
                } else {
                    // TODO
                }
                break;
            case PULSE_LBM_OPEN:
                std::cout << "FSMCONTROLLER: received PULSE_LBM_OPEN " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSorting->raiseLBM1Open();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGS_SHORT:
                std::cout << "FSMCONTROLLER: received PULSE_BGS_SHORT " << std::endl;
                if (FESTO1 == msgVal) {
                    // fsm->raiseBGS_1_INTERRUPTED();
                    fsmSystem->raiseBGS1Short();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGS_LONG:
                std::cout << "FSMCONTROLLER: received PULSE_BGS_LONG " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSystem->raiseBGS1Long();
                } else {
                    // TODO
                }
                break;
            case PULSE_BRS_SHORT:
                std::cout << "FSMCONTROLLER: received PULSE_BRS_SHORT " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSystem->raiseBRS1Short();
                    // TODO fsm->raiseBRS_1_OPEN();
                } else {
                    // TODO
                }
                break;
            case PULSE_BGR_SHORT:
                std::cout << "FSMCONTROLLER: received PULSE_BGR_SHORT " << std::endl;
                if (FESTO1 == msgVal) {
                    fsmSystem->raiseBGR1Short();
                } else {
                    // TODO
                }
                break;
            case PULSE_HS_SAMPLE:
                // std::cout << "FSMCONTROLLER: received PULSE_HS_SAMPLE FST_1" << std::endl;
                fsmHeightMeasurement->raiseHS1Sample();
                break;
            case PULSE_HS_SAMPLING_DONE:
                std::cout << "FSMCONTROLLER: received PULSE_HS_SAMPLING_DONE FST_1" << std::endl;
                fsmHeightMeasurement->raiseHS1SamplingDone();
                break;
            case PULSE_MS_TRUE:
                std::cout << "FSMCONTROLLER: received PULSE_MS_TRUE FST_1" << std::endl;
                fsmSorting->raiseMSTrue();
                break;
            case PULSE_MS_FALSE:
                std::cout << "FSMCONTROLLER: received PULSE_MS_FALSE FST_1" << std::endl;
                fsmSorting->raiseMSFalse();
                break;
            case PULSE_FSM:
                switch (msgVal) {
                case EVENT_SYSTEM_SERVICE_IN:
                    fsmLG1->raiseSystemServiceIn();
                    break;
                case EVENT_SYSTEM_SERVICE_OUT:
                    fsmLG1->raiseSystemServiceOut();
                    break;
                case EVENT_SYSTEM_OPERATIONAL_IN:
                    fsmLG1->raiseSystemOperationalIn();
                    fsmIngress->raiseSystemOperationalIn();
                    fsmHeightMeasurement->raiseSystemOperationalIn();
                    fsmSorting->raiseSystemOperationalIn();
                    fsmEgress->raiseSystemOperationalIn();
                    // TODO
                    break;
                case EVENT_SYSTEM_OPERATIONAL_OUT:
                    fsmLG1->raiseSystemOperationalOut();
                    fsmIngress->raiseSystemOperationalOut();
                    fsmHeightMeasurement->raiseSystemOperationalOut();
                    fsmEgress->raiseSystemOperationalOut();
                    // TODO
                    break;
                case EVENT_SYSTEM_ESTOP_IN:
                    std::cout << "FSMCONTROLLER: received EVENT_SYSTEM_ESTOP_IN" << std::endl;
                    fsmLR1->raiseEStopReceived();
                    fsmLY1->raiseEStopReceived();
                    fsmLG1->raiseEStopReceived();
                    fsmMotor->raiseSystemEStopIn();
                    fsmIngress->raiseSystemOperationalOut();
                    fsmHeightMeasurement->raiseSystemOperationalOut();
                    fsmSorting->raiseSystemOperationalOut();
                    fsmEgress->raiseSystemOperationalOut();
                    break;
                case EVENT_SYSTEM_ESTOP_OUT:
                    fsmLG1->raiseEStopCleared();
                    fsmLY1->raiseEStopCleared();
                    fsmLR1->raiseEStopCleared();
                    fsmMotor->raiseSystemEStopOut();
                    // fsmIngress->raiseSystemOperationalIn();
                    // fsmHeightMeasurement->raiseSystemOperationalIn();
                    // fsmSorting->raiseSystemOperationalIn();
                    // fsmEgress->raiseSystemOperationalIn();
                    break;
                case EVENT_INGRESS_PUKPRESENT_IN:
                    fsmMotor->raiseIngressPukPresentIn();
                    fsmLY1->raiseIngressIn();
                    break;
                case EVENT_INGRESS_CREATINGDISTANCE_OUT:
                    fsmMotor->raiseIngressCreatingDistanceOut();
                    break;
                case EVENT_INGRESS_IN:
                    fsmLY1->raiseIngressIn();
                    break;
                case EVENT_INGRESS_OUT:
                    fsmLY1->raiseIngressOut();
                    break;
                case EVENT_PUK_ENTRY_HEIGHT_MEASUREMENT:
                    fsmHeightMeasurement->raisePukEntryHeightMasurement();
                    break;
                case EVENT_HM_PUKPRESENT_IN:
                    fsmMotor->raiseHeightMeasurementPukPresentIn();
                    break;
                case EVENT_HM_PUKPRESENT_OUT:
                    fsmMotor->raiseHeightMeasurementPukPresentOut();
                    break;
                case EVENT_HM_MEASUREMENT_IN:
                    fsmMotor->raiseHeightMeasurementMeasurementIn();
                    break;
                case EVENT_HM_MEASUREMENT_OUT:
                    fsmMotor->raiseHeightMeasurementMeasurementOut();
                    break;
                case EVENT_HM_PUK_DISTANCE_VALID:
                    fsmIngress->raisePukDistanceValid();
                    fsmLY1->raiseIngressOut();
                    break;
                case EVENT_HM_PUK_LEAVING_HEIGHTMEASUREMENT:
                    // TODO
                    break;
                case EVENT_HM_ENTRY_SORTING:
                    fsmSorting->raisePukEntrySorting();
                    break;
                case EVENT_SORTING_PUKPRESENT_IN:
                    fsmMotor->raiseSortingPukPresentIn();
                    break;
                case EVENT_SORTING_METALMEASUREMENT_IN:

                    break;
                case EVENT_SORTING_METALMEASUREMENT_OUT:
                    fsmMotor->raiseSortingMetalMeasurementOut();
                    break;
                case EVENT_SORTING_PUK_EJECTOR_DISTANCE_VALID:
                    fsmEgress->raisePukEntryEgress();
                    break;
                case EVENT_SORTING_PUK_ENTRY_EGRESS:
                    fsmEgress->raisePukEntryEgress();
                    break;
                case EVENT_SORTING_PUKPRESENT_OUT:
                    // fsmMotor->raiseSortingPukPresentOut();
                    break;
                case EVENT_SORTING_RAMP_FULL_IN:
                    fsmMotor->raiseSortingRampFullIn();
                    break;
                case EVENT_SORTING_RAMP_FULL_OUT:
                    fsmMotor->raiseSortingRampFullOut();
                    break;
                case EVENT_SORTING_EJECTOR_OUT:
                    fsmMotor->raiseSortingEjectorOut();
                    break;
                case EVENT_SORTING_PASSTHROUGH_OUT:
                    fsmMotor->raiseSortingPassthroughOut();
                    break;
                case EVENT_EGRESS_PUKPRESENT_IN:
                    fsmMotor->raiseEgressPukPresentIn();
                    break;
                case EVENT_EGRESS_PUKPRESENT_OUT:
                    break;
                case EVENT_EGRESS_TRANSFER_IN:
                    fsmMotor->raiseEgressPukTransferIn();
                    break;
                case EVENT_EGRESS_TRANSFER_OUT:
                    fsmMotor->raiseEgressPukTransferOut();
                    break;
                default:

                    break;
                }
                break;
            default:
                std::cout << "FSMCONTROLLER: received UNKNOWN! ERROR " << std::endl;
                // TODO FEHLER
            }
        }
    }
}

int32_t FSMController::getChannel() { return channelID; }

void FSMController::sendMsg() {
    // TODO check if needed when FSMs dont send Pulses

    sc::rx::Observable<void> motorStop = fsm->getMOTOR_STOP();
    sc::rx::Observable<void> motorFast = fsm->getMOTOR_FAST();
    sc::rx::Observable<void> motorSlow = fsm->getMOTOR_SLOW();

    sc::rx::Observable<void> lg1On = fsm->getLG1_ON();
    sc::rx::Observable<void> lg1Blinking1Hz = fsm->getLG1_BLINKING_1HZ();
    sc::rx::Observable<void> lg1Off = fsm->getLG1_OFF();

    subThreadsRunning = true;
    // void monitorObservable(sc::rx::Observable<void> observable, std::string pulseName, PulseCode pulseCode, int32_t
    // pulseValue) {
    //     while (subThreadsRunning) {
    //         observable.next();
    //         if (0 > MsgSendPulse(dispatcherConnectionID, -1, pulseCode, pulseValue)) {
    //             perror("FSMController failed to send " + pulseName)
    //         }
    //     }
    // }

    // std::thread motorStopThread(FSMController::monitorObservable, motorStop, "PULSE_MOTOR1_STOP", PULSE_MOTOR1_STOP,
    // 0, subThreadsRunning, dispatcherConnectionID); std::thread motorFastThread(FSMController::monitorObservable,
    // motorFast, "PULSE_MOTOR1_FAST", PULSE_MOTOR1_FAST, 0, subThreadsRunning, dispatcherConnectionID); std::thread
    // motorSlowThread(FSMController::monitorObservable, motorSlow, "PULSE_MOTOR1_SLOW", PULSE_MOTOR1_SLOW, 0,
    // subThreadsRunning, dispatcherConnectionID); std::thread lg1OnThread(FSMController::monitorObservable, lg1On,
    // "PULSE_LG1_ON", PULSE_LG1_ON, 0, subThreadsRunning, dispatcherConnectionID); std::thread
    // lg1Blinking1HzThread(FSMController::monitorObservable, lg1Blinking1Hz, "PULSE_LG1_BLINKING", PULSE_LG1_BLINKING,
    // 1000, subThreadsRunning, dispatcherConnectionID); std::thread lg1OffThread(FSMController::monitorObservable,
    // lg1Off, "PULSE_LG1_OFF", PULSE_LG1_OFF, 0, subThreadsRunning, dispatcherConnectionID);

    // motorStopThread.join();
    // motorFastThread.join();
    // motorSlowThread.join();
    // lg1OnThread.join();
    // lg1Blinking1HzThread.join();
    // lg1OffThread.join();
}

void FSMController::monitorObservable(
    sc::rx::Observable<void> observable,
    std::string pulseName,
    PulseCode pulseCode,
    int32_t pulseValue,
    bool subThreadsRunning,
    int32_t dispatcherConnectionID
) {
    while (subThreadsRunning) {
        observable.next();
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, pulseCode, pulseValue)) {
            perror("FSMController failed to send "); // + pulseName
        }
    }
}
