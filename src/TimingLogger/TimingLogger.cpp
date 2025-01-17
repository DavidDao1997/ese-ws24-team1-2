
#include "./headers/TimingLogger.h"



int8_t TimingLogger::numOfPulses = TIMING_LOGGER_NUM_OF_PULSES;
int8_t TimingLogger::pulses[TIMING_LOGGER_NUM_OF_PULSES] = {
    PULSE_LBF_INTERRUPTED,
    PULSE_LBF_OPEN,
    PULSE_LBE_INTERRUPTED,
    PULSE_LBE_OPEN,
    PULSE_LBR_INTERRUPTED,
    PULSE_LBR_OPEN,
    PULSE_LBM_INTERRUPTED,
    PULSE_LBM_OPEN,
    PULSE_HS1_SAMPLE,
    PULSE_HS2_SAMPLE,
    PULSE_HS1_SAMPLING_DONE,
    PULSE_HS2_SAMPLING_DONE
};

TimingLogger::TimingLogger(std::string dispatcherChannelName) {
    channelID = createChannel();
    running = false;

    dispatcherConnectionID = name_open(dispatcherChannelName.c_str(), NAME_FLAG_ATTACH_GLOBAL);
}
TimingLogger::~TimingLogger() {

}

// std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

// // Simulate some processing (sleep for 2 seconds)
// std::this_thread::sleep_for(std::chrono::seconds(2));

// // Capture the end time point
// std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

// // Calculate the duration between start and end
// std::chrono::duration<int64_t, std::nano> duration = end - start;


void TimingLogger::handleMsg() {
    // setup motors
    
    

    std::chrono::high_resolution_clock::time_point timerStart;
       

    _pulse msg;
    running = true;
    bool oneHSSample1 = false;
    bool oneHSSample2 = false;

    while (running) {
        int recvid = MsgReceive(channelID, &msg, sizeof(_pulse), nullptr);

        if (recvid < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "TimingLogger");
            //exit(EXIT_FAILURE);
        }

        if (recvid == 0) { // Pulse received^
            int32_t msgVal = msg.value.sival_int;
            // Logger::getInstance().log(LogLevel::TRACE, "MSG VAL: " + std::to_string(msgVal), "ActuatorController");
            switch (msg.code) {
                case PULSE_STOP_RECV_THREAD:
                    Logger::getInstance().log(LogLevel::TRACE, " received PULSE_STOP_RECV_THREAD...", "TimingLogger");
                    running = false;
                    break;
                case PULSE_LBF_INTERRUPTED:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBF_INTERRUPTED...", "TimingLogger");
                    if (msgVal == FESTO1){
                        
                        Logger::getInstance().log(LogLevel::INFO, "Motors start", "TimingLogger");
                        WAIT(2000);
                        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_SLOW, 0)) {
                            Logger::getInstance().log(LogLevel::ERROR, "Failed to send pulse: " , "TimingLogger");
                        }
                        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR2_SLOW, 0)) {
                            Logger::getInstance().log(LogLevel::ERROR, "Failed to send pulse: " , "TimingLogger");
                        }
                    } else {
                        std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                        Logger::getInstance().log(LogLevel::INFO, "[LBE1->LBF2] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                    }
                    break;
                case PULSE_LBF_OPEN:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBF_OPEN...", "TimingLogger");
                    if (msgVal == FESTO1){
                        timerStart = std::chrono::high_resolution_clock::now();
                    } else {
                        timerStart = std::chrono::high_resolution_clock::now();
                    }
                    break;
                case PULSE_LBE_INTERRUPTED:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBE_INTERRUPTED...", "TimingLogger");
                    if (msgVal == FESTO1) {
                        std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                        Logger::getInstance().log(LogLevel::INFO, "[LBM1->LBE1] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                    } else {
                        std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                        Logger::getInstance().log(LogLevel::INFO, "[LBM2->LBE2] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR1_STOP, 0)) {
                            Logger::getInstance().log(LogLevel::ERROR, "Failed to send pulse: " , "TimingLogger");
                        }
                        if (0 < MsgSendPulse(dispatcherConnectionID, -1, PULSE_MOTOR2_STOP, 0)) {
                            Logger::getInstance().log(LogLevel::ERROR, "Failed to send pulse: " , "TimingLogger");
                        }
                    }
                    break;
                case PULSE_LBE_OPEN:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBE_OPEN...", "TimingLogger");
                    if (msgVal == FESTO1){
                        timerStart = std::chrono::high_resolution_clock::now();
                    } else {
    
                    }
                    break;
                // case PULSE_LBM_INTERRUPTED:
                //     if (msgVal == FESTO1) {
                //         std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                //         Logger::getInstance().log(LogLevel::INFO, "[HS1->LBM1] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                //     } else {
                //         std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                //         Logger::getInstance().log(LogLevel::INFO, "[HS2->LBM2] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                //     }
                //     Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBM_INTERRUPTED...", "TimingLogger");
                //     break;
                // case PULSE_LBM_OPEN:
                //     Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBM_OPEN...", "TimingLogger");
                //     // start egress section
                //     if (msgVal == FESTO1){
                //         timerStart = std::chrono::high_resolution_clock::now();
                //     } else {
                //         timerStart = std::chrono::high_resolution_clock::now();
                //     }
                //     break;
                case PULSE_HS1_SAMPLE:
                    if (!oneHSSample1) {
                        oneHSSample1 = true;
                        std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                        Logger::getInstance().log(LogLevel::INFO, "[LBF1->HS1] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                    }
                    break;
                case PULSE_HS2_SAMPLE:
                    if (!oneHSSample2) {
                        oneHSSample2 = true;
                        std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                        Logger::getInstance().log(LogLevel::INFO, "[LBF2->HS2] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                    }
                    break;
                case PULSE_HS1_SAMPLING_DONE:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_HS1_SAMPLING_DONE...", "TimingLogger");
                    oneHSSample1 = false;
                    timerStart = std::chrono::high_resolution_clock::now();
                    break;
                case PULSE_HS2_SAMPLING_DONE:
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_HS2_SAMPLING_DONE...", "TimingLogger");
                    oneHSSample2 = false;
                    timerStart = std::chrono::high_resolution_clock::now();
                    break;
                case PULSE_LBR_INTERRUPTED:
                    if (msgVal == FESTO1) {
                        std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                        Logger::getInstance().log(LogLevel::INFO, "[HS1->LBR1] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                    } else {
                        std::chrono::duration<int64_t, std::nano> duration = std::chrono::high_resolution_clock::now() - timerStart;
                        Logger::getInstance().log(LogLevel::INFO, "[HS2->LBR2] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms", "TimingLogger");
                    }
                    Logger::getInstance().log(LogLevel::TRACE, "received PULSE_LBM_INTERRUPTED...", "TimingLogger");
                    break;
                default:
                    Logger::getInstance().log(LogLevel::ERROR, " Unknown Pulse....." + std::to_string(msg.code), "TimingLogger");
            }
        } else if (msg.type == _IO_CONNECT) {
            Logger::getInstance().log(LogLevel::TRACE, ": Replying to connection...", "TimingLogger");
            MsgReply( recvid, EOK, NULL, 0 );
        }
    }
}


void TimingLogger::sendMsg() {}

int32_t TimingLogger::getChannel() {
    return channelID;
}

bool TimingLogger::stop() {
    running = false;
    // TODO destroy channel
}


int8_t* TimingLogger::getPulses(){
    return pulses;
}


int8_t TimingLogger::getNumOfPulses() {
    return numOfPulses;
};
