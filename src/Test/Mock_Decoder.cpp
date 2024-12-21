/*
 * Mock_Decoder.cpp
 *
 *  Created on: 20.12.2024
 *      Author: Marc
 */


#include "headers/Mock_Decoder.h"


Mock_Decoder::Mock_Decoder(const std::string dispatcherChannelName){
    channelID = createChannel();

    // create a connection to Dispatcher
    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), 0);

}

Mock_Decoder::~Mock_Decoder() {
    // TODO How to end thread if blocked in MsgReveivePulse
    destroyChannel(channelID);
    // TODO disconnect from dispatcher
}

void Mock_Decoder::handleMsg() {
    
}

void Mock_Decoder::decode() {

    /*
    int32_t festoId = 0;

    //static std::chrono::steady_clock::time_point pressStartTime; // Zeit, wann der Button gedrückt wurde
    //const std::chrono::milliseconds longPressDuration(LONG_PRESS_DURATION); // Definiert, was als langer Druck gilt (z. B. 1 Sekunde)

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
            perror("Dispatcher Send failed");
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
            perror("Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Berrier End
    if ((flippedValues & (uint32_t)BIT_MASK(LBE_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBE_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBE_OPEN : PULSE_LBE_INTERRUPTED;
        std::cout << "LBE erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Ramp
    if ((flippedValues & (uint32_t)BIT_MASK(LBR_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBR_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBR_OPEN : PULSE_LBR_INTERRUPTED;
        std::cout << "LBR erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }
    // Light Barrier Metal Sensor
    if ((flippedValues & (uint32_t)BIT_MASK(LBM_PIN)) != 0) {
        // LBM_PIN
        int8_t current_level = (currentValues >> LBM_PIN) & 0x1;
        int32_t code = current_level ? PULSE_LBM_OPEN : PULSE_LBM_INTERRUPTED;
        std::cout << "LBM erkannt" << std::endl;
        if (0 > MsgSendPulse(dispatcherConnectionID, -1, code, 0)) {
            perror("Dispatcher Send failed");
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
                    perror("Dispatcher Send failed");
                }
            } else {
                // Lang drücken
                std::cout << "BGS_LONG erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGS_LONG, festoId)) {
                    perror("Dispatcher Send failed");
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
                    perror("Dispatcher Send failed");
                }
            } else {
                // Lang drücken
                std::cout << "BRS_LONG erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BRS_LONG, festoId)) {
                    perror("Dispatcher Send failed");
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
                    perror("Dispatcher Send failed");
                }
            } else {
                // Lang drücken
                std::cout << "BGS_LONG erkannt" << std::endl;
                if (0 > MsgSendPulse(dispatcherConnectionID, -1, PULSE_BGR_LONG, festoId)) {
                    perror("Dispatcher Send failed");
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
            perror("Dispatcher Send failed");
        } // TODO SWITCH HERE TO SECOND FESTO (instead of 0 put 1 if festo2)
    }

    // TODO if neccessary
    // if (...) {
    //     ...
    // }
    // ...
    */
}

void Mock_Decoder::sendMsg() {
    // senden an den dispatcher
}

int32_t Mock_Decoder::getChannel() { return channelID;}


void Mock_Decoder::sendPulse(PulseCode code, uint8_t festoNr){
    if (0 < MsgSendPulse(dispatcherConnectionID, -1, code, festoNr)) {
        perror("MOCKDECODER: Sending Pulse failed\n");
    }
}