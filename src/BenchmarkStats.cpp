#include "BenchmarkStats.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

BenchmarkStats::BenchmarkStats(const std::string &tracePath)
{
    traceFile.open(tracePath);
    if (!traceFile.is_open())
    {
        throw std::runtime_error("Failed to open trace file: " + tracePath);
    }
}

BenchmarkStats::~BenchmarkStats()
{
    if (traceFile.is_open())
    {
        traceFile.close();
    }
}

void BenchmarkStats::startBenchmark()
{
    startTime = Utils::getCurrentTime();
}

void BenchmarkStats::endBenchmark()
{
    endTime = Utils::getCurrentTime();
}

void BenchmarkStats::recordRequest(uint32_t threadId, bool isRead, uint64_t offset,
                                   uint64_t bytes, double latency)
{
    std::lock_guard<std::mutex> lock(statsMutex);

    double timestamp = Utils::getCurrentTime() - startTime;
    uint64_t sectorOffset = offset / 512;
    uint64_t sectorSize = bytes / 512;

    // Write to trace file
    traceFile << std::fixed << std::setprecision(6)
              << timestamp << " "
              << threadId << " "
              << (isRead ? "R" : "W") << " "
              << sectorOffset << " "
              << sectorSize << " "
              << latency << std::endl;

    // Update statistics
    totalRequests.fetch_add(1);
    totalBytes.fetch_add(bytes);
    totalLatency += latency;
}

double BenchmarkStats::getTotalDuration() const
{
    return endTime - startTime;
}

double BenchmarkStats::getAverageLatency() const
{
    if (totalRequests == 0)
        return 0;
    return totalLatency / totalRequests;
}

uint64_t BenchmarkStats::getTotalRequests() const
{
    return totalRequests;
}

double BenchmarkStats::getIOPS() const
{
    double duration = getTotalDuration();
    if (duration <= 0)
        return 0;
    return totalRequests / duration;
}

double BenchmarkStats::getBandwidthMBps() const
{
    double duration = getTotalDuration();
    if (duration <= 0)
        return 0;
    return (totalBytes / (1024.0 * 1024.0)) / duration;
}