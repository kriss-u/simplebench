#pragma once

#include "BenchmarkConfig.h"
#include "BenchmarkStats.h"
#include <thread>
#include <atomic>
#include <cstdint>

class IOWorker
{
public:
    IOWorker(uint32_t id, const BenchmarkConfig &cfg, BenchmarkStats &st);
    ~IOWorker();

    void start();
    void stop();

private:
    void performIO();
    uint64_t getNextOffset();
    uint32_t getSectorSize() const;

    uint32_t threadId;
    const BenchmarkConfig &config;
    BenchmarkStats &stats;
    int fd;
    std::atomic<bool> running;
    std::thread workerThread;
    uint64_t currentOffset;
    uint32_t sectorSize;
};