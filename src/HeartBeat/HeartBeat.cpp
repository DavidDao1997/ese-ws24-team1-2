/*
 * HeartBeat.cpp
 *
 *  Created on: 29.11.2024
 *      Author: Marc
 */

#include "headers/HeartBeat.h"

int8_t HeartBeat::numOfPulses = HEARTBEAT_NUM_OF_PULSES;
int8_t HeartBeat::pulses[HEARTBEAT_NUM_OF_PULSES] = {
    PULSE_ESTOP_LOW,
    PULSE_ESTOP_HIGH
};

HeartBeat::HeartBeat(uint8_t festoNr, int32_t actuatorControllerChannelId) {
    running = true;
    // check if festo 1 or festo 2 in parameter list // TODO What happens if named Channel not created yet? create namedChannels in main on qnx level. who creates it then? 
    dispatcherChannel = name_open(std::string(DISPATCHERNAME).c_str(),NAME_FLAG_ATTACH_GLOBAL);
    if (0 > dispatcherChannel) {
        Logger::getInstance().log(LogLevel::CRITICAL, std::to_string(festoId) + " Could not connect to Dispatcher...", "HeartBeat");
    }
    festoId = festoNr;
    messageReceivedOnce = false;
    actChannel = actuatorControllerChannelId;
    otherFesto = -1;
    
    if (festoNr == FESTO1){
        heartBeatChannel = createNamedChannel("Festo1HeartBeat");
    } else if (festoNr == FESTO2) {
        heartBeatChannel = createNamedChannel("Festo2HeartBeat");
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Wrong ", "HeartBeat");
    }
    channelID = heartBeatChannel->chid;
}

HeartBeat::~HeartBeat(){
    if (0 > ConnectDetach(otherFesto)){
        Logger::getInstance().log(LogLevel::ERROR, "Disconnection from Dispatcher failed...", "HeartBeat");
    }
    destroyChannel(channelID);
}

void HeartBeat::connectToFesto(){
    
    // do {
    //     if (festoId == FESTO1){
    //         otherFesto = name_open("Festo2HeartBeat", NAME_FLAG_ATTACH_GLOBAL);
    //     } else if (festoId == FESTO2) {
    //         otherFesto = name_open("Festo1HeartBeat", NAME_FLAG_ATTACH_GLOBAL);
    //     } else {
    //         Logger::getInstance().log(LogLevel::ERROR, "Wrong Festo ID set " + std::to_string(festoId), "HeartBeat");
    //     }
    //     if (0 > otherFesto) {
    //         Logger::getInstance().log(LogLevel::CRITICAL, std::to_string(festoId) + " Could not connect to other Festo...", "HeartBeat");
    //     }
    //     WAIT(10);
    // } while (0 > otherFesto); 
    // Logger::getInstance().log(LogLevel::TRACE, "Connection established...", "HeartBeat");
}

bool HeartBeat::stop(){
    int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
            Logger::getInstance().log(LogLevel::CRITICAL, "Shutting down Msg Receiver failed...", "HeartBeat");
            return false;
    }
    // disconnect the connection to own channel
    Logger::getInstance().log(LogLevel::TRACE, "Shutting down PULSE send...", "HeartBeat");
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

        bool disconnect = false;
        while (running) {
            if (0 > otherFesto){ 
                if (festoId == FESTO1){
                    otherFesto = name_open("Festo2HeartBeat", NAME_FLAG_ATTACH_GLOBAL);
                } else if (festoId == FESTO2) {
                    otherFesto = name_open("Festo1HeartBeat", NAME_FLAG_ATTACH_GLOBAL);
                } else {
                    Logger::getInstance().log(LogLevel::ERROR, "Wrong Festo ID set " + std::to_string(festoId), "HeartBeat");
                }
                if (0 > otherFesto) {
                    Logger::getInstance().log(LogLevel::CRITICAL, std::to_string(festoId) + " Could not connect to other Festo...", "HeartBeat");
                    WAIT(10);
                } else {
                    if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_RECONNECT_HEARTBEAT_FESTO, festoId)) {
                        Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo1...", "HeartBeat");
                    }
                }
                if (0 >= otherFesto) {
                    disconnect = false;
                    Logger::getInstance().log(LogLevel::TRACE, "MSGSENDER ONLINE...", "HeartBeat");
                }
                WAIT(10);
                continue;
            } 
            //Logger::getInstance().log(LogLevel::DEBUG,std::to_string(festoId) + "Sending Heartbeat...", "HeartBeat");
            if (0 > MsgSendPulse(otherFesto, -1, PULSE_HEARTBEAT, 0)) {
                // Logger::getInstance().log(LogLevel::WARNING, "PulseCouldnotBeSent...", "HeartBeat");
            }
            // TODO check last heartbeat if last heartbeat < 100 ms or disconnected DONE -> WRITE TESTS
            {
                std::lock_guard<std::mutex> lock(heartbeatMutex); // Protect shared data
                auto timeSinceLastHeartbeat = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - lastHeartbeatReceived
                ).count();

                if (disconnect == false && timeSinceLastHeartbeat > 3000 && messageReceivedOnce) { // Adjust threshold as necessary
                    Logger::getInstance().log(LogLevel::WARNING, "Heartbeat overdue. Disconnecting...", "HeartBeat");
                    handleDisconnect(); // Handle disconnection
                    disconnect = true;
                } else if  (disconnect == true){
                    Logger::getInstance().log(LogLevel::DEBUG, "ATTEMPTING RECONNECT", "FOOOOO");
                    if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_RECONNECT_HEARTBEAT_FESTO, festoId)) {
                        Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo1...", "HeartBeat");
                        WAIT(1000);
                    } else {
                        disconnect = false;
                        Logger::getInstance().log(LogLevel::DEBUG, "RECONNECTED", "FOOOOO");
                    }   
                }
            }
            WAIT(10);
        }
    }


void HeartBeat::handleMsg() {
    ThreadCtl(_NTO_TCTL_IO, 0); // Request IO privileges

    _pulse msg;
    running = true;

    Logger::getInstance().log(LogLevel::TRACE, "MSGHANDLER ONLINE...", "HeartBeat");
    while (running) {
        int recvid = MsgReceive(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "HeartBeat");
        }

        if (recvid == 0) { // Pulse received
            if (msg.code == PULSE_HEARTBEAT) {
                //Logger::getInstance().log(LogLevel::DEBUG, "Getting Heartbeat from Other Festo...", "HeartBeat");
                // TODO update time when last heartbeat was received. 
                {
                    std::lock_guard<std::mutex> lock(heartbeatMutex); // Protect shared data
                    lastHeartbeatReceived = std::chrono::steady_clock::now(); // Update heartbeat timestamp
                    if(messageReceivedOnce == false)Logger::getInstance().log(LogLevel::DEBUG, "MessageReceiveOnce First Heartbeat!!", "HeartBeat");
                    messageReceivedOnce = true;
                }      
            } else if (msg.code == PULSE_STOP_RECV_THREAD) {
                Logger::getInstance().log(LogLevel::TRACE, "received PULSE_STOP_RECV_THREAD...", "HeartBeat");
                running = false;
            // } else if (msg.code == PULSE_ESTOP_LOW) {
            //     Logger::getInstance().log(LogLevel::TRACE, "received PULSE_ESTOP_LOW...", "HeartBeat");
            //     eStopPressed = true;     
            // } else if (msg.code == PULSE_ESTOP_HIGH) {
            //     Logger::getInstance().log(LogLevel::TRACE, "received PULSE_ESTOP_HIGH...", "HeartBeat");
            //     eStopPressed = false;                   
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "Unexpected Pulse received: " + std::to_string(msg.code), "HeartBeat");
            }
        } else if (msg.type == _IO_CONNECT) {
            Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoId) + ": Replying to connection...", "HeartBeat");
            MsgReply( recvid, EOK, NULL, 0 );
        }
    }
}

void HeartBeat::handleDisconnect() {
    Logger::getInstance().log(LogLevel::CRITICAL, "Connection to other Festo LOST...", "HeartBeat");
    // when FESTO 1: send PULSE_E_STOP_HEARTBEAT_FESTO1 to Dispatcher ONCE and if E_STOP Button pressend, send this also als value == 1, else value == 0
    if (festoId == FESTO1){
        if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_E_STOP_HEARTBEAT_FESTO1, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo1...", "HeartBeat");
        }        
    }
    // when FESTO 2: send PULSE_E_STOP_HEARTBEAT_FESTO2 to Dispatcher until reachable and manually set Actuators
    if (festoId == FESTO2){
        if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_E_STOP_HEARTBEAT_FESTO2, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }

        // TODO set Actuators
        int32_t coid = connectToChannel(actChannel);
        if (0 > MsgSendPulse(coid, -1, PULSE_LG2_ON, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }
         if (0 > MsgSendPulse(coid, -1, PULSE_LR2_ON, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }
         if (0 > MsgSendPulse(coid, -1, PULSE_LY2_ON, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }
        if (0 > MsgSendPulse(coid, -1, PULSE_MOTOR2_STOP, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }
        

    }
}

int32_t HeartBeat::getChannel() { return channelID; }