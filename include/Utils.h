#pragma once
#include <cstdint>
#include <string>

namespace Utils
{
    double getCurrentTime();
    uint64_t bytesToSectors(uint64_t bytes);
    uint64_t getRandomOffset(uint64_t range, uint64_t requestSize);
    double getElapsedTime(double startTime);
}