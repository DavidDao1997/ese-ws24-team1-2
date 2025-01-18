/*
 * PositionTracker.h
 *
 *  Created on: 11.01.2025
 *      Author: Marc
 */

#ifndef FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_
#define FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_

#include <queue>
#include <chrono>
#include <list>
#include <mutex>
#include <map>
#include <tuple>

#include "../../gen/src-gen/FSM.h"
#include "../../gen/src/sc_rxcpp.h"
#include "../../../Logging/headers/Logger.h"
#include "../../../Util/headers/Util.h"
#include "../../Puk/headers/Puk.h"
#include "../../Timer/headers/Timer.h"

// these are measured times by hand with a stopwatch, without any added margin
//#define DURATION_INGRESS_DISTANCE_VALID_FAST std::chrono::milliseconds(1240) // guessed as DURATION_HS_PUK_EXPECTED_FAST / 2
//#define DURATION_INGRESS_DISTANCE_VALID_SLOW std::chrono::milliseconds(2790) // guessed as DURATION_HS_PUK_EXPECTED_SLOW / 2
//#define DURATION_HS_PUK_EXPECTED_FAST std::chrono::milliseconds(2480)
// #define DURATION_HS_PUK_EXPECTED_SLOW std::chrono::milliseconds(5580)
// #define DURATION_HS_PUK_EXPIRED_FAST std::chrono::milliseconds(3500)
// #define DURATION_HS_PUK_EXPIRED_SLOW std::chrono::milliseconds(7400) // worst case measured, no margin added yet

#define TESTING_INGRESS_FAST 1000

#define TESTING_INGRESS_DISTANCE_FAST 1000
#define TESTING_HS_FAST 2000

#define TESTING_SORTING_FAST 1000

#define TESTING_SORTING_DISTANCE_FAST 1000
#define TESTING_EGRESS_FAST 2000

struct Duration {
    // LBE_OPEN -> LBF_INTERRUPTED
    struct Ingress {
        struct Expected {
            static constexpr std::chrono::milliseconds Fast{100};
            static constexpr std::chrono::milliseconds Slow{200};
        };
        struct Expired {
            static constexpr std::chrono::milliseconds Fast{15000};
            static constexpr std::chrono::milliseconds Slow{10000};
        };
        struct Mean {
            static constexpr std::chrono::milliseconds Fast = (Duration::Ingress::Expected::Fast + Duration::Ingress::Expired::Fast) / 2;
            static constexpr std::chrono::milliseconds Slow = (Duration::Ingress::Expected::Slow + Duration::Ingress::Expired::Slow) / 2;
        };

        // LBF_OPEN -> DISTANCE_CLEARED
        struct DistanceValid {
            static constexpr std::chrono::milliseconds Fast{100};
            static constexpr std::chrono::milliseconds Slow{2000};
        };
    };

    // LBF_OPEN -> HS_SAMPLE
    struct HeightSensor {
        struct Expected {
            static constexpr std::chrono::milliseconds Fast{200};
            static constexpr std::chrono::milliseconds Slow{400};
        };
        struct Expired {
            static constexpr std::chrono::milliseconds Fast{5000};
            static constexpr std::chrono::milliseconds Slow{7000};
        };
        struct Mean {
            static constexpr std::chrono::milliseconds Fast = (Duration::HeightSensor::Expected::Fast + Duration::HeightSensor::Expired::Fast) / 2;
            static constexpr std::chrono::milliseconds Slow = (Duration::HeightSensor::Expected::Slow + Duration::HeightSensor::Expired::Slow) / 2;
        };
    };

    // HS_SAMPLING_DONE -> LBM_INTERRUPTED
    struct Sorting {
        struct Expected {
            static constexpr std::chrono::milliseconds Fast{500};
            static constexpr std::chrono::milliseconds Slow{1000};
        };
        struct Expired {
            static constexpr std::chrono::milliseconds Fast{2000};
            static constexpr std::chrono::milliseconds Slow{3500};
        };
        struct Mean {
            static constexpr std::chrono::milliseconds Fast = (Duration::Sorting::Expected::Fast + Duration::Sorting::Expired::Fast) / 2;
            static constexpr std::chrono::milliseconds Slow = (Duration::Sorting::Expected::Slow + Duration::Sorting::Expired::Slow) / 2;
        };

        // LBM_OPEN -> DIVERTER_CLEARED
        struct DistanceValid {
            static constexpr std::chrono::milliseconds Fast{500};
            static constexpr std::chrono::milliseconds Slow{1000};
        };
    };

    // LBM_OPEN -> LBE_INTERRUPTED
    struct Egress {
        struct Expected {
            static constexpr std::chrono::milliseconds Fast{1500};
            static constexpr std::chrono::milliseconds Slow{5000};
        };
        struct Expired {
            static constexpr std::chrono::milliseconds Fast{4000};
            static constexpr std::chrono::milliseconds Slow{7000};
        };
        struct Mean {
            static constexpr std::chrono::milliseconds Fast = (Duration::Egress::Expected::Fast + Duration::Egress::Expired::Fast) / 2;
            static constexpr std::chrono::milliseconds Slow = (Duration::Egress::Expected::Slow + Duration::Egress::Expired::Slow) / 2;
        };
    };
};

struct TimerProps {
    timer_t timerid;
    struct itimerspec timer;
    // uint32_t progress;
};

class PositionTracker{
public:
    PositionTracker(FSM* fsm);
    ~PositionTracker();
    
    

    enum SegmentType {
        SEGMENT_INGRESS,
        SEGMENT_HS,
        SEGMENT_SORTING, 
        SEGMENT_EGRESS
    };
    enum DurationType {
        DURATION_VALID,
        DURATION_EXPECTED,
        DURATION_EXPIRED
    };
private:

    FSM* fsm;
    void listen();
    std::thread* listenThread;
    uint32_t connectionId;

    std::mutex heightSensor1Mutex;
    std::queue<Puk*> heightSensor1;  // LBF_1_OPEN -> HS1_SAMPLE
    std::mutex sorting1Mutex;
    std::queue<Puk*> sorting1;       // HS1_SAMPLING_DONE -> LBM_1_INTERRUPTED
    std::mutex egress1Mutex;
    std::queue<Puk*> egress1;        // LBM_1_OPEN -> LBE_1_INTERRUPTED

    
    std::mutex ingress2Mutex;
    std::queue<Puk*> ingress2;      // LBE_1_OPEN -> LBF_2_INTERRUPTED
    std::mutex heightSensor2Mutex;
    std::queue<Puk*> heightSensor2;  // LBF_2_OPEN -> HS2_SAMPLE
    std::mutex sorting2Mutex;
    std::queue<Puk*> sorting2;       // HS2_SAMPLING_DONE -> LBM_2_INTERRUPTED
    std::mutex egress2Mutex;
    std::queue<Puk*> egress2;        // LBM_2_OPEN -> LBE_2_INTERRUPTED

    std::atomic<uint32_t> lastPukId;
    uint32_t nextPukId();

    bool isMetalDesired1;
    bool isMetalDesired2;

    std::atomic<Timer::MotorState> motorState1;
    std::atomic<Timer::MotorState> motorState2;

    void handleMotorChange(uint8_t festoId, Timer::MotorState motorState);

    std::chrono::milliseconds getDuration(SegmentType segmentType, DurationType durationType, Timer::MotorState motorState);
    std::queue<Puk*> updatePukQueue(std::queue<Puk*> tempQueue, Timer::MotorState ms);
    Puk* queuePop(std::queue<Puk*>* queue);

    void onEvent(sc::rx::Observable<void>* event ,std::function<void()> callback);
};

class PositionTrackerObserver : public sc::rx::Observer<void> {
public:
    // Constructor that accepts a callback of type void() (a function with no parameters and no return value)
    PositionTrackerObserver(std::function<void()> callback): callback(callback) {}

    // Override the next() method to invoke the callback
    virtual void next() override {
        callback();
    }

private:
    std::function<void()> callback;
};


#endif /* FSM_POSITIONTRACKER_HEADERS_POSITIONTRACKER_H_ */
