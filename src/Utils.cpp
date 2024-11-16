#include "Utils.h"
#include <random>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace
{
    // Private namespace for internal variables
    std::chrono::time_point<std::chrono::steady_clock> programStartTime =
        std::chrono::steady_clock::now();
}

namespace Utils
{

    double getCurrentTime()
    {
        auto now = std::chrono::steady_clock::now();
        // Return seconds elapsed since program start
        return std::chrono::duration<double>(now - programStartTime).count();
    }

    uint64_t bytesToSectors(uint64_t bytes)
    {
        return bytes / 512;
    }

    uint64_t getRandomOffset(uint64_t range, uint64_t requestSize)
    {
        // Thread-local random number generator
        static thread_local std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint64_t> dist(0, range - requestSize);
        return dist(rng);
    }

     double getElapsedTime(double startTime)
    {
        return getCurrentTime() - startTime;
    }

}