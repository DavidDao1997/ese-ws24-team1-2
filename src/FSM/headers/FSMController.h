#ifndef FSMCONTROLLER_H_
#define FSMCONTROLLER_H_

#include <functional> // For std::bind

#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"
#include "../gen/src-gen/FSM.h"
#include "../../Util/headers/Util.h"
#include "../../Logging/headers/Logger.h"
#include "../PositionTracker/headers/PositionTracker.h"
#include "../gen/src/sc_rxcpp.h"

#define FSM_CONTROLLER_NUM_OF_PULSES 26

class FSMController : public PulseMsgHandler {
  public:
    FSMController(const std::string dispatcherChannelName);
    virtual ~FSMController();

    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    int8_t *getPulses();
    int8_t getNumOfPulses();
    bool stop();

  private:
    void subscribeToOutEvents();
    void handlePulse(_pulse msg);
    FSM *fsm;
  
    int32_t channelID;
    int32_t dispatcherConnectionID;

    bool running;
    bool subThreadsRunning;
    static int8_t numOfPulses;
    static int8_t pulses[FSM_CONTROLLER_NUM_OF_PULSES];

    class VoidObserver : public sc::rx::Observer<void> {
    public:
        // Constructor that accepts a callback of type void() (a function with no parameters and no return value)
        VoidObserver(
            int coid,
            int code,
            int value,
            const std::string& name
        ): coid(coid), code(code), value(value), name(name) {}

        // Override the next() method to invoke the callback
        virtual void next() override {
            Logger::getInstance().log(LogLevel::TRACE, "Sending pulse: " + name, "FSMController");
            if (0 < MsgSendPulse(coid, -1, code, value)) {
                Logger::getInstance().log(LogLevel::ERROR, "Failed to send pulse: " + name, "FSMController");
            }
        }

    private:
        int coid;
        int code;
        int value;
        std::string name;
    };
};

#endif /* LOGIK_FSM_FSMCONTROLLER_H_ */
