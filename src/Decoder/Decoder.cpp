/*
 * Decoder.cpp
 *
 *  Created on: 10.11.2024
 *      Author: Marc
 */

#include "headers/Decoder.h"
#include "../HAL/headers/HALConfig.h"
#include <cstdio> // For sprintf


#define LONG_PRESS_DURATION 1000

Decoder::Decoder(const std::string dispatcherChannelName) {
    running = false;

    channelID = createChannel();

    sensorISR = new SensorISR();
    // sonsorISR = foo;
    if (!sensorISR->registerInterrupt(channelID)) {
        perror("decoder could not register interrupt");
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
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);

    // TODO check if festo 1 or festo 2 in parameter list
}

Decoder::~Decoder() {
    if (0 < MsgSendPulse(channelID, -1, PULSE_STOP_RECV_THREAD, 0)) {
        perror("DECODER: shutting down Msg Receiver failed");
    } else {
        std::cout << "DECODER: Shutting down PULSE send " << std::endl;
    }


    //running = false;
    // TODO How to end thread if blocked in MsgReveivePulse
    //destroyChannel(channelID);
    // TODO disconnect from dispatcher
}

void Decoder::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("DECODER: MsgReceivePulse failed!"); // TODO does this happen on decoonstruct???
            running = false;
            return; // TODO
        }

        if (recvid == 0) { // Pulse received
            if (msg.code != PULSE_INTR_ON_PORT0) {
                std::cout << "DECODER: received PULSE_STOP_RECV_THREAD " << std::endl;
                running = false;
                std::cout << "DECODER: destroying own channel " << std::endl;
                destroyChannel(channelID);                        
            }
            else if (msg.code != PULSE_INTR_ON_PORT0) {
                perror("DECODER: Unexpected Pulse!");
            } else {
                decode();
                std::cout << "DECODER: Interrupt received entpacken und weitergabe an dispatcher" << std::endl;
            }
        }
    }
}

void Decoder::decode() {
    int32_t festoId = 0;

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
    std::cout << "decode" << std::endl;

    // ESTOP
    if ((flippedValues & (uint32_t)BIT_MASK(SES_PIN)) != 0) {
        // SES_PIN
        int8_t current_level = (currentValues >> SES_PIN) & 0x1;
        int32_t code = current_level ? PULSE_ESTOP_HIGH : PULSE_ESTOP_LOW;
        // Test
        char buffer[100];
        sprintf(buffer, "DECODER: current level %d\n", current_level);
        std::cout << buffer << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("DECODER: Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Front
    if ((flippedValues & (uint32_t)BIT_MASK(LBF_PIN)) != 0) {
        // LBF_PIN
        uint8_t current_level = (currentValues >> LBF_PIN) & 0x1;
        // TODO Add 2 Festo support eg add festo# to value instead of sending just 0 and remove number ->
        // PULSE_LBF_INTERRUPTED
        int32_t code = current_level ? PULSE_LBF_OPEN : PULSE_LBF_INTERRUPTED;
        std::cout << "LBF erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("DECODER: Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Berrier End
    if ((flippedValues & (uint32_t)BIT_MASK(LBE_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBE_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBE_OPEN : PULSE_LBE_INTERRUPTED;
        std::cout << "LBE erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("DECODER: Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Ramp
    if ((flippedValues & (uint32_t)BIT_MASK(LBR_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBR_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBR_OPEN : PULSE_LBR_INTERRUPTED;
        std::cout << "LBR erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("DECODER: Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Metal Sensor
    if ((flippedValues & (uint32_t)BIT_MASK(LBM_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBM_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBM_OPEN : PULSE_LBM_INTERRUPTED;
        std::cout << "LBM erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("DECODER: Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Button Start
    if ((flippedValues & (uint32_t)BIT_MASK(BGS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> BGS_PIN) & 0x1; // HIGH wenn bestätigt
        if (current_level) {
            std::cout << "BGS gedrueckt" << std::endl;
            pressStartTime = std::chrono::steady_clock::now(); // Startzeit des Tastendrucks
        } else {
            std::cout << "BGS losgelassen" << std::endl;
            // timer stuff to check if it was short or long press
            auto pressDuration = std::chrono::steady_clock::now() - pressStartTime; // Berechnung der Dauer
            if (pressDuration < longPressDuration) {
                // Kurz drücken
                std::cout << "BGS_SHORT erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_SHORT, festoId)) {
                    perror("DECODER: Dispatcher Send failed");
                }
            } else {
                // Lang drücken
                std::cout << "BGS_LONG erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_LONG, festoId)) {
                    perror("DECODER: Dispatcher Send failed");
                }
            }
        }
    }
    // Button Stop
    if ((flippedValues & (uint32_t)BIT_MASK(BRS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> BRS_PIN) & 0x1; // LOW wenn bestätigt
        if (!current_level) {
            std::cout << "BGS gedrueckt" << std::endl;
            pressStartTime = std::chrono::steady_clock::now(); // Startzeit des Tastendrucks
        } else {
            std::cout << "BGS losgelassen" << std::endl;
            // timer stuff to check if it was short or long press
            auto pressDuration = std::chrono::steady_clock::now() - pressStartTime; // Berechnung der Dauer
            if (pressDuration < longPressDuration) {
                // Kurz drücken
                std::cout << "BRS_SHORT erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_SHORT, festoId)) {
                    perror("DECODER: Dispatcher Send failed");
                }
            } else {
                // Lang drücken
                std::cout << "BRS_LONG erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_LONG, festoId)) {
                    perror("DECODER: Dispatcher Send failed");
                }
            }
        }
    }
    // Button Reset
    if ((flippedValues & (uint32_t)BIT_MASK(BGR_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> BGR_PIN) & 0x1; // HIGH wenn bestätigt
        if (current_level) {
            std::cout << "BGS gedrueckt" << std::endl;
            pressStartTime = std::chrono::steady_clock::now(); // Startzeit des Tastendrucks
        } else {
            std::cout << "BGS losgelassen" << std::endl;
            // timer stuff to check if it was short or long press
            auto pressDuration = std::chrono::steady_clock::now() - pressStartTime; // Berechnung der Dauer
            if (pressDuration < longPressDuration) {
                // Kurz drücken
                std::cout << "BGS_SHORT erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_SHORT, festoId)) {
                    perror("DECODER: Dispatcher Send failed");
                }
            } else {
                // Lang drücken
                std::cout << "BGS_LONG erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_LONG, festoId)) {
                    perror("DECODER: Dispatcher Send failed");
                }
            }
        }
    }

    // Metal Sensor
    if ((flippedValues & (uint32_t)BIT_MASK(MS_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> MS_PIN) & 0x1;
        int32_t code = current_level ? PULSE_MS_TRUE : PULSE_MS_FALSE;
        std::cout << "MS erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("DECODER: Dispatcher Send failed");
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
