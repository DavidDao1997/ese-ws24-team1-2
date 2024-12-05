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
}

Dispatcher::~Dispatcher() {
    // Send stop pulse to terminate any dispatch thread
    int connectionID = ConnectAttach(0, 0, channelID, _NTO_SIDE_CHANNEL, 0);
    if (connectionID >= 0) {
        MsgSendPulse(connectionID, -1, PULSE_STOP_THREAD, 0);
        ConnectDetach(connectionID);
    } else {
        perror("Failed to connect for stop pulse!");
    }

    for (int32_t coid : connections) {
        ConnectDetach(coid);
    }

    // Destroy the channel
    destroyNamedChannel(channelID, dispatcherChannel);
}

void Dispatcher::addSubscriber(int32_t chid, int8_t pulses[], int8_t numOfPulses) {
    int coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
    if (coid < 0) {
        perror("Failed to connect!");
        return;
    }
    connections.push_back(coid);
    for (uint8_t i = 0; i < numOfPulses; ++i) {
        uint8_t pulse = pulses[i];
        connectionsByPulse[pulse].push_back(coid);
    }
}

void Dispatcher::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    bool running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE); // TODO exit??? was passiert wenn der dispatcher stirbt. fehlerbhandlung?
        }

        if (recvid == 0) { // Pulse received
            switch (msg.code) {
            case PULSE_STOP_THREAD:
                running = false;
                break;
                // case PULSE_DISPATCHER_SUBSCRIBE:
                // std::cout << "subscribing" << std::endl;
                //  coids add msg.coid;
                //  events = msg.events;
                /*
                map[key]value
                {
                        "10234": PULSE_DISPATCHER_SUBSCRIBE, PULSE_DISPATCHER_SUBSCRIBE,PULSE_DISPATCHER_SUBSCRIBE
                }
                */
                // break;
            default:
                char buffer[100];
                sprintf(buffer, "DISPATCHER: Revieved pulse %d\n", msg.code);
                std::cout << buffer << std::flush;

                // Schaue in map wer sich für msg.code interessiert und schicke an diese
                auto coids = connectionsByPulse.find(msg.code);
                if (coids != connectionsByPulse.end()) {
                    for (const auto &coid : coids->second) {
                        sprintf(buffer, "DISPATCHER: Forwarding pulse %d to connectionId %d\n", msg.code, coid);
                        std::cout << buffer << std::flush;
                        int err = MsgSendPulse(coid, -1, msg.code, 0);
                        if (err == -1) {
                            perror("DISPACHER: MsgSendPulse failed");
                        }
                    }
                }
            }
        }
    }
}

void Dispatcher::sendMsg() {}

int32_t Dispatcher::getChannel() { return channelID; }
