/*
 * HeartBeat.cpp
 *
 *  Created on: 29.11.2024
 *      Author: Marc
 */

#include "headers/HeartBeat.h"

HeartBeat::HeartBeat() {
    running = false;
    // TODO check if festo 1 or festo 2 in parameter list
}

HeartBeat::~HeartBeat(){
    running = false;
}

bool HeartBeat::init(int32_t hbOtherFesto) {
    int32_t channelIDSecondFesto = hbOtherFesto;
    int32_t coid = ConnectAttach(0, 0, channelIDSecondFesto, _NTO_SIDE_CHANNEL, 0);
    if (coid == -1) {
        Logger::getInstance().log(LogLevel::WARNING, "Other Festo not Found...", "HeartBeat");
        return false;
    }
    return true;
}

void HeartBeat::sendMsg() {
    // TODO: Send heartbeat and check when last heartbeat was received. if received heartbeat is
    // overdue then handleDisconnect, else reset timer

    // int msg = -1; // TODO CHANGE CODE
    // int status = MsgSendPulse(coid, msg, code, value);
    // if (status == -1) {
    //    perror("MsgSendPulse");
    //}
}

void HeartBeat::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    running = true;

    while (running) {
        int recvid = MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "HeartBeat");
        }

        if (recvid == 0) { // Pulse received
            if (msg.code == PULSE_HEARTBEAT) {
                // TODO update time when last heartbeat was received.
                Logger::getInstance().log(LogLevel::DEBUG, "Getting Heartbeat from Slave...", "HeartBeat");
            }
        }
    }
}

void HeartBeat::handleDisconnect() {
    // TODO
}

int32_t HeartBeat::getChannel() { return channelID; }