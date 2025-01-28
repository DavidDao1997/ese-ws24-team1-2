/*
 * HeartBeat.cpp
 *
 *  Created on: 29.11.2024
 *      Author: Marc
 */
 
#include "headers/HeartBeat.h"
 
#define MILLION 1000000

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
    coidAct = connectToChannel(actChannel);
    if (0 > coidAct){
        Logger::getInstance().log(LogLevel::WARNING, "act channel not reachable(coid = -1)...", "HeartBeat");
    } 
    otherFesto = -1;
    sendCnt = 0;

    
    if (festoId == FESTO1){
        heartBeatChannel = createNamedChannel("Festo1HeartBeat");
    } else if (festoId == FESTO2) {
        heartBeatChannel = createNamedChannel("Festo2HeartBeat");
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Wrong Festo ID set " + std::to_string(festoId), "HeartBeat");
    }
    channelID = heartBeatChannel->chid;

    
    // if (festoNr == FESTO1){
    //     heartBeatChannel = createNamedChannel("Festo1HeartBeat");
    // } else if (festoNr == FESTO2) {
    //     heartBeatChannel = createNamedChannel("Festo2HeartBeat");
    // } else {
    //     Logger::getInstance().log(LogLevel::ERROR, "Wrong ", "HeartBeat");
    // }
    // channelID = heartBeatChannel->chid;
}
 
 
 
HeartBeat::~HeartBeat(){
    if (0 > ConnectDetach(otherFesto)){
        Logger::getInstance().log(LogLevel::ERROR, "Disconnection from Dispatcher failed...", "HeartBeat");
    }
    destroyChannel(channelID);
}
 
void HeartBeat::connectToFesto(){
    if(0 > heartBeatChannel){
        if (festoId == FESTO1){
            heartBeatChannel = createNamedChannel("Festo1HeartBeat");
        } else if (festoId == FESTO2) {
            heartBeatChannel = createNamedChannel("Festo2HeartBeat");
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Wrong Festo ID set " + std::to_string(festoId), "HeartBeat");
        }
        channelID = heartBeatChannel->chid;
    }
    {
        std::lock_guard<std::mutex> lock(otherFestoMutex);
        if (festoId == FESTO1){
            otherFesto = name_open("Festo2HeartBeat", NAME_FLAG_ATTACH_GLOBAL);
        } else if (festoId == FESTO2) {
            otherFesto = name_open("Festo1HeartBeat", NAME_FLAG_ATTACH_GLOBAL);
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Wrong Festo ID set " + std::to_string(festoId), "HeartBeat");
        }
    }
    // if (0 > otherFesto) {
    //     Logger::getInstance().log(LogLevel::CRITICAL, std::to_string(festoId) + " Could not connect to other Festo...", "HeartBeat");
    // }
   
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
        bool disconnect = true;

       
        while (running) {
            if (0 > otherFesto){
                connectToFesto();
                if (0 > otherFesto) {
                    
                    // WAIT(10);
                } else {
                    if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_RECONNECT_HEARTBEAT_FESTO, festoId)) {
                        Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo1...", "HeartBeat");
                    }
                }
                if (otherFesto >= 0) {
                    disconnect = false;
                    Logger::getInstance().log(LogLevel::TRACE, "MSGSENDER ONLINE AND OTHER FESTO FOUND...", "HeartBeat");
                }
                WAIT(100);
                continue;
            }
            // Logger::getInstance().log(LogLevel::DEBUG,std::to_string(festoId) + "Sending Heartbeat...", "HeartBeat");
            if (0 > MsgSendPulse(otherFesto, -1, PULSE_HEARTBEAT, 0)) {
                Logger::getInstance().log(LogLevel::WARNING, "PulseCouldnotBeSent..., sendCnt = " + std::to_string(sendCnt), "HeartBeat");
                std::lock_guard<std::mutex> lock(sendCntMutex);
                sendCnt++;
            } else {
                std::lock_guard<std::mutex> lock(sendCntMutex);
                sendCnt = 0;
            }
            if (!messageReceivedOnce) {
                // Logger::getInstance().log(LogLevel::DEBUG, "NOT YET HEARD FROM OTHER FESTO", "Heartbeat");
                WAIT(100);
                continue;
            }
            WAIT(100);
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
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed..." + std::to_string(recvid) + " " + std::to_string(channelID), "HeartBeat");
        }
 
        if (recvid == 0) { // Pulse received
            if (msg.code == PULSE_HEARTBEAT) {
                // Logger::getInstance().log(LogLevel::DEBUG, "Getting Heartbeat from Other Festo...", "HeartBeat");
                //  update time when last heartbeat was received.
                {
                    std::lock_guard<std::mutex> lock(heartbeatMutex); // Protect shared data
                    lastHeartbeatReceived = std::chrono::steady_clock::now(); // Update heartbeat timestamp
                }      
                if(messageReceivedOnce == false)Logger::getInstance().log(LogLevel::DEBUG, "MessageReceiveOnce First Heartbeat!!", "HeartBeat");
                messageReceivedOnce = true;
            } else if (msg.code == PULSE_STOP_RECV_THREAD) {
                Logger::getInstance().log(LogLevel::TRACE, "received PULSE_STOP_RECV_THREAD...", "HeartBeat");
                running = false;              
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "Unexpected Pulse received: " + std::to_string(msg.code), "HeartBeat");
            }
        } else if (msg.type == _IO_CONNECT) {
            Logger::getInstance().log(LogLevel::TRACE, std::to_string(festoId) + ": Replying to connection...", "HeartBeat");
            MsgReply( recvid, EOK, NULL, 0 );
        }
    }
}

void HeartBeat::checkConnection(){
    Logger::getInstance().log(LogLevel::DEBUG, "Heartbeat checking thread starting...", "HeartBeat");
    bool disconnect = false;
    int32_t disconnectChannelID = createChannel();
    int32_t connectionId = ConnectAttach(0, 0, disconnectChannelID, 0, 0);
    if (0 > connectionId){
        Logger::getInstance().log(LogLevel::WARNING, "Timer connect FAILED...", "HeartBeat");
    }
    //Logger::getInstance().log(LogLevel::DEBUG, "disconnectChannelID: " + std::to_string(disconnectChannelID)+" connectionId: " + std::to_string(connectionId), "");
    struct sigevent event;
        // setup pulse
    SIGEV_PULSE_INIT(&event, connectionId, SIGEV_PULSE_PRIO_INHERIT, _PULSE_CODE_MINAVAIL + 1, 0);
    timer_t timerId;
    if (0 > timer_create (CLOCK_REALTIME, &event, &timerId)){
        Logger::getInstance().log(LogLevel::WARNING, "Timer create FAILED...", "HeartBeat");
    } // Fehlerbehandlung fehlt
    struct itimerspec timerSpec = {};
    timerSpec.it_value.tv_sec = 1;
    timerSpec.it_value.tv_nsec = 0;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = (500) * MILLION;

    if (0 > timer_settime (timerId, 0, &timerSpec, NULL)) {
        Logger::getInstance().log(LogLevel::WARNING, "Timer setting FAILED...", "HeartBeat");
    }
    Logger::getInstance().log(LogLevel::DEBUG, "Heartbeat checking online...", "HeartBeat");

    while (running){
        _pulse msg;

        //int recvid = MsgReceivePulse(disconnectChannelID, &msg, sizeof(_pulse), nullptr);
        //Logger::getInstance().log(LogLevel::DEBUG, "MsgRecevedd", "HeartBeat");
         
        // if (recvid < 0) {
        //     Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed..." + std::to_string(recvid) + " " + std::to_string(channelID), "HeartBeat.checkConnection");
        // }
        WAIT(20);
        if (messageReceivedOnce ) { // Pulse received    && recvid == 0
        
            int timeSinceLastHeartbeat;
            {
                std::lock_guard<std::mutex> lock(heartbeatMutex); // Protect shared data
                timeSinceLastHeartbeat = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - lastHeartbeatReceived
                ).count();
            }
            //Logger::getInstance().log(LogLevel::DEBUG, "timeSinceLastHeartbeat Heartbeat checking ..." + std::to_string(timeSinceLastHeartbeat), "HeartBeat");

            {   
                std::lock_guard<std::mutex> lock(sendCntMutex);
                if (!disconnect && sendCnt > 5) { // Adjust threshold as necessary timeSinceLastHeartbeat > 500
                    Logger::getInstance().log(LogLevel::WARNING, "Heartbeat overdue. Disconnecting...", "HeartBeat.checkConnection");
                    handleDisconnect(); // Handle disconnection
                    {
                        std::lock_guard<std::mutex> lock(otherFestoMutex);
                        //otherFesto = -1; needed for reconnect behaviour
                    }
                    disconnect = true;        
                } else if  (disconnect == true && sendCnt <= 5){  //&& timeSinceLastHeartbeat < 500

                    Logger::getInstance().log(LogLevel::DEBUG, "RECONNECT To Dispatcher", "HeartBeat");
                    disconnect = false;
                    // dispatcherChannel = name_open(std::string(DISPATCHERNAME).c_str(),NAME_FLAG_ATTACH_GLOBAL);
                    // if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_RECONNECT_HEARTBEAT_FESTO, festoId)) {
                    //     Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PULSE_RECONNECT_HEARTBEAT_FESTO Could not be send...", "HeartBeat");
                    //     WAIT(1000);
                    // } else {
                    //     disconnect = false;
                    //     Logger::getInstance().log(LogLevel::DEBUG, "RECONNECTED", "HeartBeat");
                    // }  
                }
            }  
        } else {
            //Logger::getInstance().log(LogLevel::DEBUG, "recvid Heartbeat checking ..." + std::to_string(recvid), "HeartBeat");
        }
    }
}
 
void HeartBeat::handleDisconnect() {
    Logger::getInstance().log(LogLevel::CRITICAL, "Connection to other Festo LOST...", "HeartBeat");
    // when FESTO 1: send PULSE_E_STOP_HEARTBEAT_FESTO1 to Dispatcher ONCE and if E_STOP Button pressend, send this also als value == 1, else value == 0
   
    if (festoId == FESTO1){
        if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_E_STOP_HEARTBEAT_FESTO1, festoId)) {
              //  set Actuators

            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo1...", "HeartBeat");

            if (0 > MsgSendPulse(coidAct, -1, PULSE_MOTOR1_STOP, festoId)) {
                Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
            }
            if (0 > MsgSendPulse(coidAct, -1, PULSE_LG1_OFF, festoId)) {
                Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
            }
            if (0 > MsgSendPulse(coidAct, -1, PULSE_LR1_BLINKING, 500)) {
                Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
            } else {
                Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "Pulse send Festo2...", "HeartBeat");
            }
            if (0 > MsgSendPulse(coidAct, -1, PULSE_LY1_OFF, festoId)) {
                Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
            }      
        }  

    }
    
    // when FESTO 2: send PULSE_E_STOP_HEARTBEAT_FESTO2 to Dispatcher until reachable and manually set Actuators
    if (festoId == FESTO2){
        // if (0 > MsgSendPulse(dispatcherChannel, -1, PULSE_E_STOP_HEARTBEAT_FESTO2, festoId)) {
        //     Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        // }

        //  set Actuators
        // int32_t coid = connectToChannel(actChannel);
        // if (0 > coidAct){
        //     Logger::getInstance().log(LogLevel::WARNING, "act channel not reachable(coid = -1)...", "HeartBeat");
        // } 
        if (0 > MsgSendPulse(coidAct, -1, PULSE_MOTOR2_STOP, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }
        if (0 > MsgSendPulse(coidAct, -1, PULSE_LG2_OFF, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }
        if (0 > MsgSendPulse(coidAct, -1, PULSE_LR2_BLINKING, 500)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        } else {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "Pulse send Festo2...", "HeartBeat");
        }
        if (0 > MsgSendPulse(coidAct, -1, PULSE_LY2_OFF, festoId)) {
            Logger::getInstance().log(LogLevel::WARNING, std::to_string(festoId) + "PulseCouldnotBeSent Festo2...", "HeartBeat");
        }
    }
}
 
int32_t HeartBeat::getChannel() { return channelID; }
 