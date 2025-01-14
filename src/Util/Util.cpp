#include "headers/Util.h"


void fireAt(std::chrono::steady_clock::time_point start, int milliseconds) {
    // Calculate the target time (current time + the required duration)
    auto targetTime = start + std::chrono::milliseconds(milliseconds);
    
    // Sleep until the target time is reached
    std::this_thread::sleep_until(targetTime);
}