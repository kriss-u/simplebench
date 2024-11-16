#include <IOWorker.h>
#include <Utils.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
#include <cstring>

IOWorker::IOWorker(int id, const BenchmarkConfig &cfg, BenchmarkStats &st)
    : threadId(id),
      config(cfg),
      stats(st),
      fd(-1),
      running(false),  // Initialize running flag
      currentOffset(0) // Initialize currentOffset
{
    std::cout << "Creating worker " << id << std::endl;

    int flags = O_RDWR; // Read and write mode
    if (config.useDirectIO)
    {
        flags |= O_DIRECT; // Use O_DIRECT flag
        std::cout << "Using O_DIRECT for worker " << id << std::endl;
    }

    fd = open(config.devicePath.c_str(), flags);
    if (fd < 0)
    {
        std::cerr << "Failed to open device " << config.devicePath
                  << " for worker " << id
                  << ": " << strerror(errno) << std::endl;
        throw std::runtime_error("Device open failed.");
    }
    std::cout << "Worker " << id << " opened fd: " << fd << std::endl;
}

IOWorker::~IOWorker()
{
    std::cout << "Destructor for worker " << threadId << std::endl;
    if (running)
    {
        stop();
    }
    if (fd >= 0)
    {
        std::cout << "Closing fd " << fd << " for worker " << threadId << std::endl;
        close(fd);
    }
}

void IOWorker::start()
{
    if (!running)
    {
        running = true;
        // Properly store the thread object
        workerThread = std::thread(&IOWorker::performIO, this);
    }
}

void IOWorker::stop()
{
    std::cout << "Stopping worker " << threadId << std::endl;
    running = false;
    if (workerThread.joinable())
    {
        std::cout << "Joining thread for worker " << threadId << std::endl;
        workerThread.join();
    }
    std::cout << "Worker " << threadId << " stopped" << std::endl;
}

void IOWorker::performIO()
{
    std::cout << "Worker " << threadId << " starting IO" << std::endl;

    // Check file descriptor
    if (fd < 0)
    {
        std::cerr << "Invalid file descriptor in worker " << threadId << std::endl;
        return;
    }

    // Align buffer size to 512-byte sectors
    const size_t alignedSize = ((config.requestKB * 1024 + 511) / 512) * 512;
    std::cout << "Aligned size: " << alignedSize << " bytes" << std::endl;

    void *alignedBuf = nullptr;
    if (posix_memalign(&alignedBuf, 512, alignedSize) != 0)
    {
        std::cerr << "Buffer alignment failed for worker " << threadId << std::endl;
        return;
    }
    std::cout << "Buffer aligned successfully for worker " << threadId << std::endl;

    // Initialize buffer to prevent potential memory issues
    memset(alignedBuf, 0, alignedSize);

    try
    {
        while (running)
        {
            uint64_t offset = (getNextOffset() / 512) * 512;

            // Add bounds checking
            if (offset >= config.rangeMB * 1024 * 1024)
            {
                std::cerr << "Offset " << offset << " exceeds range "
                          << (config.rangeMB * 1024 * 1024) << std::endl;
                continue;
            }

            double startTime = Utils::getCurrentTime();
            ssize_t result;

            if (config.isRead)
            {
                result = pread(fd, alignedBuf, alignedSize, offset);
            }
            else
            {
                result = pwrite(fd, alignedBuf, alignedSize, offset);
            }

            if (result < 0)
            {
                std::cerr << "I/O error in worker " << threadId
                          << " at offset " << offset
                          << ": " << strerror(errno) << std::endl;
            }
            else
            {
                double latency = Utils::getCurrentTime() - startTime;
                stats.recordRequest(result, latency * 1000);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in worker " << threadId
                  << ": " << e.what() << std::endl;
    }

    std::cout << "Worker " << threadId << " cleaning up" << std::endl;
    free(alignedBuf);
}

uint64_t IOWorker::getNextOffset()
{
    if (config.isSequential)
    {
        uint64_t offset = currentOffset;
        currentOffset += config.requestKB * 1024;
        if (currentOffset >= config.rangeMB * 1024 * 1024)
        {
            currentOffset = 0;
        }
        return offset;
    }
    return Utils::getRandomOffset(config.rangeMB * 1024 * 1024,
                                  config.requestKB * 1024);
}