#include "BenchmarkStats.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

void BenchmarkStats::startBenchmark()
{
    startTime = Utils::getCurrentTime();
}

void BenchmarkStats::endBenchmark()
{
    endTime = Utils::getCurrentTime();
}

void BenchmarkStats::recordRequest(uint64_t bytes, double latency)
{
    // Use the mutex to protect both the stats update and console output
    std::lock_guard<std::mutex> lock(statsMutex);

    // Format output before printing to avoid interleaving
    std::ostringstream oss;
    oss << "Recording request: " << bytes << " bytes, latency "
        << std::fixed << std::setprecision(6) << latency << " ms" << std::endl;

    // Atomic operations for counters
    totalRequests.fetch_add(1);
    totalBytes.fetch_add(bytes);
    totalLatency += latency;

    // Single atomic write to cout
    std::cout << oss.str();
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