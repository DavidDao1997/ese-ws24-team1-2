/*
 * Decoder.cpp
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#include "headers/Decoder.h"
#include "../HAL/headers/HALConfig.h"
// #include <cstdio> // For sprintf TODO REMOVE


#define LONG_PRESS_DURATION 1000

Decoder::Decoder(const std::string dispatcherChannelName, uint8_t festoID) {
    Logger::getInstance().log(LogLevel::DEBUG, "Initializing Decoder", "Decoder");
    running = false;

    channelID = createChannel();

    sensorISR = new SensorISR();
    // sonsorISR = foo;
    if (!sensorISR->registerInterrupt(channelID)) {
        Logger::getInstance().log(LogLevel::CRITICAL, "Could not register interrupt...", "Decoder");
    }
    // Buttons
    sensorISR->initializeGPIOInterrupt(BGS_PIN);
    sensorISR->initializeGPIOInterrupt(BRS_PIN);
    sensorISR->initializeGPIOInterrupt(BGR_PIN);

    // LightBarrier
    sensorISR->initializeGPIOInterrupt(LBF_PIN);
    sensorISR->initializeGPIOInterrupt(LBE_PIN);
    sensorISR->initializeGPIOInterrupt(LBM_PIN);
    sensorISR->initializeGPIOInterrupt(LBR_PIN);

    // EStop
    sensorISR->initializeGPIOInterrupt(SES_PIN);

    // MetalSensor
    sensorISR->initializeGPIOInterrupt(MS_PIN);

    // create a connection to Dispatcher
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), NAME_FLAG_ATTACH_GLOBAL);
    Logger::getInstance().log(LogLevel::DEBUG, "NAME_FLAG_ATTACH_GLOBAL: " + std::to_string(NAME_FLAG_ATTACH_GLOBAL), "Decoder");
    Logger::getInstance().log(LogLevel::DEBUG, "dispatcherConnectionID: " + std::to_string(dispatcherConnectionID), "Decoder");

    // TODO check if festo 1 or festo 2 in parameter list
    if ((festoID == FESTO1) || (festoID == FESTO2)){
        festoNr = festoID;
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Wrong Festo Nr in Constructor...", "Decoder");
    }
}

Decoder::~Decoder() {
    // TODO disconnect from dispatcher
    if (0 > ConnectDetach(dispatcherConnectionID)){
        Logger::getInstance().log(LogLevel::ERROR, "Disconnection from Dispatcher failed...", "Decoder");
    }
    // TODO How to end thread if blocked in MsgReveivePulse and return avlue?
    destroyChannel(channelID);
}

bool Decoder::stop(){
	int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
            Logger::getInstance().log(LogLevel::CRITICAL, "Shutting down Msg Receiver failed...", "Decoder");
            return false;
    }
    // disconnect the connection to own channel
    Logger::getInstance().log(LogLevel::TRACE, "Shutting down PULSE send...", "Decoder");
    if (0 > ConnectDetach(coid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "Decoder");
        return false;
    }
    return true;
}

void Decoder::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "Decoder"); // TODO does this happen on decoonstruct???
            running = false;
            return; // TODO
        }

        if (recvid == 0) { // Pulse received
            if (msg.code == PULSE_STOP_RECV_THREAD) {
                Logger::getInstance().log(LogLevel::TRACE, "received PULSE_STOP_RECV_THREAD...", "Decoder");
                running = false;                      
            }
            else if (msg.code != PULSE_INTR_ON_PORT0) {
                Logger::getInstance().log(LogLevel::WARNING, "Unexpected Pulse received...", "Decoder");
            } else {
                decode();
                Logger::getInstance().log(LogLevel::TRACE, "Interrupt received entpacken und weitergabe an dispatcher...", "Decoder");
            }
        }
    }
}

void Decoder::decode() {
    uint8_t festoId = festoNr;

    uint32_t flippedValues = sensorISR->getFlippedValues();
    uint32_t currentValues = sensorISR->getCurrentValues();
    sensorISR->clearCurrentInterrupt();

    static std::chrono::steady_clock::time_point pressStartTime; // Zeit, wann der Button gedrückt wurde
    const std::chrono::milliseconds longPressDuration(LONG_PRESS_DURATION
    ); // Definiert, was als langer Druck gilt (z. B. 1 Sekunde)

    // char buffer1[100];
    // sprintf(
    //     buffer1,
    //     "DECODER: flippededValue %x\n\t getCurrentValue %x",
    //     (flippedValues & (uint32_t)BIT_MASK(LBF_PIN)),
    //     (currentValues & (uint32_t)BIT_MASK(LBF_PIN))
    // );
    Logger::getInstance().log(LogLevel::TRACE, "decode...", "Decoder");

    // ESTOP
    if ((flippedValues & (uint32_t)BIT_MASK(SES_PIN)) != 0) {
        // SES_PIN
        int8_t current_level = (currentValues >> SES_PIN) & 0x1;
        int32_t code = current_level ? PULSE_ESTOP_HIGH : PULSE_ESTOP_LOW;
        // Test
        Logger::getInstance().log(LogLevel::TRACE, "current level: " + std::to_string(current_level), "Decoder");
        // char buffer[100];
        // sprintf(buffer, "DECODER: current level %d\n", current_level);
        // std::cout << buffer << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, festoId)) {
            Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed, send to heartbeat", "Decoder");
            
        } // TODO 
    }
    // Light Barrier Front
    if ((flippedValues & (uint32_t)BIT_MASK(LBF_PIN)) != 0) {
        // LBF_PIN
        uint8_t current_level = (currentValues >> LBF_PIN) & 0x1;
        // TODO Add 2 Festo support eg add festo# to value instead of sending just 0 and remove number ->
        // PULSE_LBF_INTERRUPTED
        int32_t code = current_level ? PULSE_LBF_OPEN : PULSE_LBF_INTERRUPTED;
        Logger::getInstance().log(LogLevel::TRACE, "LBF erkannt", "Decoder");
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, festoId)) {
            Logger::getInstance().log(LogLevel::TRACE, "Dispatcher Send failed", "Decoder");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Berrier End
    if ((flippedValues & (uint32_t)BIT_MASK(LBE_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBE_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBE_OPEN : PULSE_LBE_INTERRUPTED;
        Logger::getInstance().log(LogLevel::TRACE, "LBE erkannt", "Decoder");
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, festoId)) {
            Logger::getInstance().log(LogLevel::TRACE, "Dispatcher Send failed", "Decoder");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Ramp
    if ((flippedValues & (uint32_t)BIT_MASK(LBR_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBR_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBR_OPEN : PULSE_LBR_INTERRUPTED;
        Logger::getInstance().log(LogLevel::TRACE, "LBR erkannt", "Decoder");
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, festoId)) {
            Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Metal Sensor
    if ((flippedValues & (uint32_t)BIT_MASK(LBM_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBM_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBM_OPEN : PULSE_LBM_INTERRUPTED;
        Logger::getInstance().log(LogLevel::TRACE, "LBM erkannt", "Decoder");
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, festoId)) {
            Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Button Start
    if ((flippedValues & (uint32_t)BIT_MASK(BGS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> BGS_PIN) & 0x1; // HIGH wenn bestätigt
        if (current_level) {
            Logger::getInstance().log(LogLevel::TRACE, "BGS gedrueckt", "Decoder");
            pressStartTime = std::chrono::steady_clock::now(); // Startzeit des Tastendrucks
        } else {
            Logger::getInstance().log(LogLevel::TRACE, "BGS losgelassen", "Decoder");
            // timer stuff to check if it was short or long press
            auto pressDuration = std::chrono::steady_clock::now() - pressStartTime; // Berechnung der Dauer
            if (pressDuration < longPressDuration) {
                // Kurz drücken
                Logger::getInstance().log(LogLevel::TRACE, "BGS_SHORT erkannt", "Decoder");
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, festoId)) {
                    Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
                }
            } else {
                // Lang drücken
                Logger::getInstance().log(LogLevel::TRACE, "BGS_LONG erkannt", "Decoder");
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_LONG, festoId)) {
                    Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
                }
            }
        }
    }
    // Button Stop
    if ((flippedValues & (uint32_t)BIT_MASK(BRS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> BRS_PIN) & 0x1; // LOW wenn bestätigt
        if (!current_level) {
            Logger::getInstance().log(LogLevel::TRACE, "BRS gedrueckt", "Decoder");
            pressStartTime = std::chrono::steady_clock::now(); // Startzeit des Tastendrucks
        } else {
            Logger::getInstance().log(LogLevel::TRACE, "BRS losgelassen", "Decoder");
            // timer stuff to check if it was short or long press
            auto pressDuration = std::chrono::steady_clock::now() - pressStartTime; // Berechnung der Dauer
            if (pressDuration < longPressDuration) {
                // Kurz drücken
                Logger::getInstance().log(LogLevel::TRACE, "BRS_SHORT erkannt", "Decoder");
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_SHORT, festoId)) {
                    Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
                }
            } else {
                // Lang drücken
                Logger::getInstance().log(LogLevel::TRACE, "BRS_LONG erkannt", "Decoder");
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_LONG, festoId)) {
                    Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
                }
            }
        }
    }
    // Button Reset
    if ((flippedValues & (uint32_t)BIT_MASK(BGR_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> BGR_PIN) & 0x1; // HIGH wenn bestätigt
        if (current_level) {
            Logger::getInstance().log(LogLevel::TRACE, "BGR gedrueckt", "Decoder");
            pressStartTime = std::chrono::steady_clock::now(); // Startzeit des Tastendrucks
        } else {
            Logger::getInstance().log(LogLevel::TRACE, "BGR losgelassen", "Decoder");
            // timer stuff to check if it was short or long press
            auto pressDuration = std::chrono::steady_clock::now() - pressStartTime; // Berechnung der Dauer
            if (pressDuration < longPressDuration) {
                // Kurz drücken
                Logger::getInstance().log(LogLevel::TRACE, "BGR_SHORT erkannt", "Decoder");
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_SHORT, festoId)) {
                    Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
                }
            } else {
                // Lang drücken
                Logger::getInstance().log(LogLevel::TRACE, "BGR_LONG erkannt", "Decoder");
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_LONG, festoId)) {
                    Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
                }
            }
        }
    }

    // Metal Sensor
    if ((flippedValues & (uint32_t)BIT_MASK(MS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> MS_PIN) & 0x1;
        int32_t code = current_level ? PULSE_MS_TRUE : PULSE_MS_FALSE;
        Logger::getInstance().log(LogLevel::TRACE, "MS erkannt", "Decoder");
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, festoId)) {
            Logger::getInstance().log(LogLevel::ERROR, "Dispatcher Send failed", "Decoder");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }

    // TODO if neccessary
    // if (...) {
    //     ...
    // }
    // ...
}

void Decoder::sendMsg() {
    // senden an den dispatcher
}

int32_t Decoder::getChannel() { return channelID; }
