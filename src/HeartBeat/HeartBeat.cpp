/*
 * HeartBeat.cpp
 *
 *  Created on: 29.11.2024
 *      Author: Marc
 */

#include "headers/HeartBeat.h"

HeartBeat::HeartBeat(uint8_t festoNr) {
    running = false;
    // check if festo 1 or festo 2 in parameter list // TODO What happens if named Channel not created yet? create namedChannels in main on qnx level. who creates it then? 
    if (festoNr == FESTO1){
        heartBeatChannel = createNamedChannel("Festo1HeartBeat");
        otherFesto = name_open("Festo2HeartBeat", 0);
    } else if (festoNr == FESTO2) {
        heartBeatChannel = createNamedChannel("Festo2HeartBeat");
        otherFesto = name_open("Festo1HeartBeat", 0);
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Wrong ", "HeartBeat");
    }

    channelID = heartBeatChannel->chid;
    if (0 > otherFesto) {
        Logger::getInstance().log(LogLevel::CRITICAL, "Could not connect to other Festo...", "HeartBeat");
    }
}

HeartBeat::~HeartBeat(){
    if (0 > ConnectDetach(otherFesto)){
        Logger::getInstance().log(LogLevel::ERROR, "Disconnection from Dispatcher failed...", "HeartBeat");
    }
    destroyChannel(channelID);
}

bool HeartBeat::stop(){
    int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
            Logger::getInstance().log(LogLevel::CRITICAL, "Shutting down Msg Receiver failed...", "HeartBeat");
            return false;
    }
    // disconnect the connection to own channel
    Logger::getInstance().log(LogLevel::DEBUG, "Shutting down PULSE send...", "HeartBeat");
    if (0 > ConnectDetach(coid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "HeartBeat");
        return false;
    }
    return true;
}

void HeartBeat::sendMsg() {
    // TODO: Send heartbeat and check when last heartbeat was received. if received heartbeat is
    // overdue then handleDisconnect, else reset timer
    // TODO: check for first connect. initial connection handling

    while (running) {
        if (0 > MsgSendPulse(otherFesto, -1, PULSE_HEARTBEAT, 0)) {
            Logger::getInstance().log(LogLevel::WARNING, "PulseCouldnotBeSent...", "HeartBeat");
        }
        // TODO check last heartbeat if last heartbeat < 100 ms or disconnected
        {
            std::lock_guard<std::mutex> lock(heartbeatMutex); // Protect shared data
            auto timeSinceLastHeartbeat = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastHeartbeatReceived
            ).count();

            if (timeSinceLastHeartbeat > 100) { // Adjust threshold as necessary
                Logger::getInstance().log(LogLevel::WARNING, "Heartbeat overdue. Disconnecting...", "HeartBeat");
                handleDisconnect(); // Handle disconnection
            }
        }

        WAIT(10);
    }
    
    

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
                Logger::getInstance().log(LogLevel::DEBUG, "Getting Heartbeat from Other Festo...", "HeartBeat");
                // TODO update time when last heartbeat was received. 
                {
                    std::lock_guard<std::mutex> lock(heartbeatMutex); // Protect shared data
                    lastHeartbeatReceived = std::chrono::steady_clock::now(); // Update heartbeat timestamp
                }        

            } else if (msg.code == PULSE_STOP_RECV_THREAD) {
                Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_STOP_RECV_THREAD...", "HeartBeat");
                running = false;                      
            } else {
                Logger::getInstance().log(LogLevel::WARNING, "Unexpected Pulse received...", "HeartBeat");
            }
        }
    }
}

void HeartBeat::handleDisconnect() {
    // TODO
    Logger::getInstance().log(LogLevel::CRITICAL, "Connection to other Festo LOST...", "HeartBeat");
}

int32_t HeartBeat::getChannel() { return channelID; }