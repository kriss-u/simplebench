#ifndef BENCHMARK_STATS_H
#define BENCHMARK_STATS_H

#include <atomic>
#include <mutex>
#include <chrono>

class BenchmarkStats
{
private:
    std::atomic<uint64_t> totalRequests{0};
    std::atomic<uint64_t> totalBytes{0};
    double totalLatency{0.0}; // in milliseconds
    double startTime{0.0};
    double endTime{0.0};
    std::mutex statsMutex;

public:
    void startBenchmark();
    void endBenchmark();
    void recordRequest(uint64_t bytes, double latency);
    double getTotalDuration() const;
    double getAverageLatency() const;
    uint64_t getTotalRequests() const;
    double getIOPS() const;
    double getBandwidthMBps() const;
};

#endif