#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstdint>

namespace Utils
{
    // Function declarations
    double getCurrentTime();
    uint64_t bytesToSectors(uint64_t bytes);
    uint64_t getRandomOffset(uint64_t range, uint64_t requestSize);
    std::string formatTraceLine(double timestamp, int threadId,
                                bool isRead, uint64_t offset,
                                uint64_t size, double latency);
    double getElapsedTime(double startTime);
}

#endif