#pragma once
#include <atomic>
#include <mutex>
#include <fstream>
#include <string>
#include <cstdint>

class BenchmarkStats
{
private:
    std::atomic<uint64_t> totalRequests{0};
    std::atomic<uint64_t> totalBytes{0};
    double totalLatency{0};
    double startTime{0};
    double endTime{0};
    std::mutex statsMutex;
    std::ofstream traceFile;

public:
    explicit BenchmarkStats(const std::string &tracePath);
    ~BenchmarkStats();

    // Disable copy operations
    BenchmarkStats(const BenchmarkStats &) = delete;
    BenchmarkStats &operator=(const BenchmarkStats &) = delete;

    void startBenchmark();
    void endBenchmark();
    void recordRequest(uint32_t threadId, bool isRead, uint64_t offset,
                       uint64_t bytes, double latency);

    double getTotalDuration() const;
    double getAverageLatency() const;
    uint64_t getTotalRequests() const;
    double getIOPS() const;
    double getBandwidthMBps() const;
};