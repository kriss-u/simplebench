#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include "ArgumentParser.h"
#include "BenchmarkConfig.h"
#include "BenchmarkStats.h"
#include "IOWorker.h"

int main(int argc, char *argv[])
{
    try
    {
        BenchmarkConfig config = ArgumentParser::parse(argc, argv);
        config.validate();

        BenchmarkStats stats(config.outputPath);
        std::vector<std::unique_ptr<IOWorker>> workers;

        stats.startBenchmark();

        // Create and start workers
        for (uint32_t i = 0; i < config.numThreads; i++)
        {
            auto worker = std::make_unique<IOWorker>(i, config, stats);
            worker->start();
            workers.push_back(std::move(worker));
        }

        // Wait for the benchmark duration
        std::cout << "Running benchmark for " << config.duration << " seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(config.duration));

        // Stop workers and record end time
        std::cout << "Stopping workers..." << std::endl;
        for (auto &worker : workers)
        {
            worker->stop();
        }

        // Record end time before clearing workers
        stats.endBenchmark();

        // Clear workers vector (this will call destructors)
        workers.clear();

        // Print results with better formatting
        std::cout << "\nBenchmark Results:\n"
                  << "================\n"
                  << std::fixed << std::setprecision(3)
                  << "Duration: " << stats.getTotalDuration() << "s\n"
                  << "Total Requests: " << stats.getTotalRequests() << "\n"
                  << "Average Latency: " << stats.getAverageLatency() << "ms\n"
                  << "IOPS: " << stats.getIOPS() << "\n"
                  << "Bandwidth: " << stats.getBandwidthMBps() << "MB/s\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}