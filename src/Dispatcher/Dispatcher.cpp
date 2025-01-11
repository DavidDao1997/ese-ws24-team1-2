/*
 * Dispatcher.cpp
 *
 *  Created on: 21.11.2024
 *      Author: Marc
 */

#include "headers/Dispatcher.h"

Dispatcher::Dispatcher(const std::string name) {
    dispatcherChannel = createNamedChannel(name);
    channelID = dispatcherChannel->chid;
    running = false;
}

Dispatcher::~Dispatcher() {
    for (int32_t coid : connections) {
         ConnectDetach(coid);
    }

    // Destroy the channel
    destroyNamedChannel(channelID, dispatcherChannel);
}



void Dispatcher::addSubscriber(int32_t chid, int8_t pulses[], int8_t numOfPulses) {
    int coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
    if (coid < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to connect!...", "Dispatcher");
        return;
    }
    connections.push_back(coid);
    for (uint8_t i = 0; i < numOfPulses; ++i) {
        uint8_t pulse = pulses[i];
        connectionsByPulse[pulse].push_back(coid);
    }
}

void Dispatcher::addKnownSubscriber(std::string channelName){
    if ((0 == strcmp(channelName.c_str(), "actuatorController1")) || (0 == strcmp(channelName.c_str(), "actuatorController2"))){
        Logger::getInstance().log(LogLevel::DEBUG, "Connecting ActuatorController..." + channelName, "Dispatcher");
        int32_t connectionID = name_open(channelName.c_str(), NAME_FLAG_ATTACH_GLOBAL);
        connections.push_back(connectionID);
        for (uint8_t i = 0; i <  ActuatorController::numOfPulses; ++i) {
            uint8_t pulse = ActuatorController::pulses[i];
            connectionsByPulse[pulse].push_back(connectionID);
        }
    }
}

bool Dispatcher::stop(){
	int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
            Logger::getInstance().log(LogLevel::CRITICAL, "Shutting down Msg Receiver failed...", "Dispatcher");
            return false;
    }
    // disconnect the connection to own channel
    Logger::getInstance().log(LogLevel::DEBUG, "Shutting down PULSE send...", "Dispatcher");
    if (0 > ConnectDetach(coid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "Dispatcher");
        return false;
    }
    return true;
}

void Dispatcher::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceive(channelID, &msg, sizeof(_pulse), nullptr);
        if (recvid < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "Dispatcher");
            //exit(EXIT_FAILURE); // TODO exit??? was passiert wenn der dispatcher stirbt. fehlerbhandlung?
        }

        if (recvid == 0) { // Pulse received
            int32_t msgVal = msg.value.sival_int;
            switch (msg.code) {
                case PULSE_STOP_RECV_THREAD:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_STOP_RECV_THREAD...", "Dispatcher");
                    running = false;
                    break;
                case PULSE_SUBSCRIBE:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_SUBSCRIBE...", "Dispatcher");
                    if (FESTO1 == msgVal){
                        addKnownSubscriber("actuatorController1");
                    } else if(FESTO2 == msgVal){
                        addKnownSubscriber("actuatorController2");
                    }
                    break;
                default:
                    Logger::getInstance().log(LogLevel::TRACE, "Dispatcher forwarding messages", "Dispatcher");
                    // char buffer[100];
                    // sprintf(buffer, "DISPATCHER: Revieved pulse %d\n", msg.code);
                    // std::cout << buffer << std::flush;

                    // Schaue in map wer sich für msg.code interessiert und schicke an diese
                    auto coids = connectionsByPulse.find(msg.code);
                    if (coids != connectionsByPulse.end()) {
                        for (const auto &coid : coids->second) {
                            // sprintf(buffer, "DISPATCHER: Forwarding pulse %d to connectionId %d\n", msg.code, coid);
                            // std::cout << buffer << std::flush;
                            int err = MsgSendPulse(coid, -1, msg.code, msg.value.sival_int);
                            if (err == -1) {
                                Logger::getInstance().log(LogLevel::ERROR, "MsgSendPulse failed...", "Dispatcher");
                            }
                        }
                    }
            }
        } else if (msg.type == _IO_CONNECT) {
            Logger::getInstance().log(LogLevel::TRACE, "Replying to connection...", "Dispatcher");
            MsgReply( recvid, EOK, NULL, 0 );
        }
    }
}

void Dispatcher::sendMsg() {}

int32_t Dispatcher::getChannel() { return channelID; }
