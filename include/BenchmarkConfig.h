#pragma once

#include <string>
#include <cstdint> // For fixed-width integer types

struct BenchmarkConfig
{
    uint32_t duration;      // Test duration in seconds
    std::string devicePath; // Target device/file path
    uint32_t rangeMB;       // Range in megabytes
    uint32_t requestKB;     // Request size in kilobytes
    bool isRead;            // true for read, false for write
    bool isSequential;      // true for sequential, false for random
    uint32_t numThreads;    // Number of threads (queue depth)
    bool useDirectIO;       // Use O_DIRECT flag for file I/O
    std::string outputPath; // Output trace file path

    // Default constructor
    BenchmarkConfig() : duration(0), rangeMB(0), requestKB(0),
                        isRead(false), isSequential(false), numThreads(1),
                        useDirectIO(false) {}
};
