#pragma once

#include <thread>
#include <atomic>
#include <BenchmarkConfig.h>
#include <BenchmarkStats.h>

class IOWorker
{
private:
    int threadId;
    int fd; // File descriptor
    BenchmarkConfig config;
    BenchmarkStats &stats;
    std::atomic<bool> running{true}; // Atomic flag for thread termination
    uint64_t currentOffset{0};       // For sequential access
    std::thread workerThread;

public:
    IOWorker(int id, const BenchmarkConfig &cfg, BenchmarkStats &st);
    ~IOWorker();
    void start();
    void stop();

private:
    void performIO();
    uint64_t getNextOffset();
};