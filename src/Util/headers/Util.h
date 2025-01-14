#ifndef UTIL_H
#define UTIL_H

// for wait
#include <chrono>
#include <thread>
#include "../../FSM/gen/src-gen/FSM.h"
#include "../../Logging/headers/Logger.h"

#define WAIT(x) (std::this_thread::sleep_for(std::chrono::milliseconds(x)))

#define FESTO1 0
#define FESTO2 1

#define DISPATCHERNAME "dispatcher"

// fires milliseconds after start, i.e. calling it twice in a row witht ehsame values will fire at the same time
void fireAt(std::chrono::steady_clock::time_point start, int milliseconds);

#endif
