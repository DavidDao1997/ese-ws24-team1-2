#ifndef UTIL_H
#define UTIL_H

// for wait
#include <chrono>
#include <thread>

#define WAIT(x) (std::this_thread::sleep_for(std::chrono::milliseconds(x)))

#endif
