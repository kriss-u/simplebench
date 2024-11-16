#pragma once
#include <string>
#include <stdexcept>
#include <cstdint>

struct BenchmarkConfig
{
    uint32_t duration; // seconds
    std::string devicePath;
    uint32_t rangeMB;
    uint32_t requestKB;
    bool isRead;
    bool isSequential;
    uint32_t numThreads; // 1-32 threads
    bool useDirectIO;
    std::string outputPath;

    void validate() const;
};
