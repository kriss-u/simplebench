#include <iostream>
#include <getopt.h>
#include <BenchmarkConfig.h>
#include <BenchmarkStats.h>
#include <IOWorker.h>
#include <vector>
#include <iomanip>

BenchmarkConfig parseArgs(int argc, char *argv[])
{
    BenchmarkConfig config;
    int opt;
    while ((opt = getopt(argc, argv, "e:f:r:s:t:p:q:d:o:")) != -1)
    {
        switch (opt)
        {
        case 'e':
            config.duration = std::stoi(optarg);
            break;
        case 'f':
            config.devicePath = optarg;
            break;
        case 'r':
            config.rangeMB = std::stoi(optarg);
            break;
        case 's':
            config.requestKB = std::stoi(optarg);
            break;
        case 't':
            config.isRead = (optarg[0] == 'R');
            break;
        case 'p':
            config.isSequential = (optarg[0] == 'S');
            break;
        case 'q':
            config.numThreads = std::stoi(optarg);
            break;
        case 'd':
            config.useDirectIO = (optarg[0] == 'T');
            break;
        case 'o':
            config.outputPath = optarg;
            break;
        default:
            throw std::runtime_error("Invalid arguments");
        }
    }
    return config;
}

int main(int argc, char *argv[])
{
    try
    {
        BenchmarkConfig config = parseArgs(argc, argv);
        BenchmarkStats stats;
        std::vector<std::unique_ptr<IOWorker>> workers;

        // Start timing before creating workers
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