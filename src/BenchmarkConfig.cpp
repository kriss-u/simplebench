#include "BenchmarkConfig.h"

void BenchmarkConfig::validate() const
{
    if (requestKB < 1 || requestKB > 512)
    {
        throw std::runtime_error("Request size must be between 1KB and 512KB");
    }
    if (numThreads < 1 || numThreads > 32)
    {
        throw std::runtime_error("Number of threads must be between 1 and 32");
    }
    if (duration == 0)
    {
        throw std::runtime_error("Duration must be positive");
    }
    if (rangeMB == 0)
    {
        throw std::runtime_error("Range must be positive");
    }
    if (outputPath.empty())
    {
        throw std::runtime_error("Output trace file path is required");
    }
}