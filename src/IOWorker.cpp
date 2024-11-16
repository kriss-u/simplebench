#include "IOWorker.h"
#include "Utils.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <iostream>
#include <cstring>
#include <errno.h>

uint32_t IOWorker::getSectorSize() const
{
    int sector_size = 0;

    // Try to get physical sector size first
    if (ioctl(fd, BLKSSZGET, &sector_size) >= 0)
    {
        return static_cast<uint32_t>(sector_size);
    }

    // If that fails, try logical sector size
    if (ioctl(fd, BLKBSZGET, &sector_size) >= 0)
    {
        return static_cast<uint32_t>(sector_size);
    }

    // If both fail, default to standard 512 bytes
    std::cerr << "Warning: Could not detect sector size, defaulting to 512 bytes" << std::endl;
    return 512;
}

IOWorker::IOWorker(uint32_t id, const BenchmarkConfig &cfg, BenchmarkStats &st)
    : threadId(id), config(cfg), stats(st), fd(-1), running(false), currentOffset(0)
{
    int flags = O_RDWR;
    if (config.useDirectIO)
    {
        flags |= O_DIRECT;
    }

    fd = open(config.devicePath.c_str(), flags);
    if (fd < 0)
    {
        throw std::runtime_error("Failed to open device: " + std::string(strerror(errno)));
    }

    // Get sector size right after opening the device
    sectorSize = getSectorSize();
    std::cout << "Thread " << threadId << " detected sector size: " << sectorSize << " bytes" << std::endl;
}

IOWorker::~IOWorker()
{
    if (running)
    {
        stop();
    }
    if (fd >= 0)
    {
        close(fd);
    }
}

void IOWorker::start()
{
    if (!running)
    {
        running = true;
        workerThread = std::thread(&IOWorker::performIO, this);
    }
}

void IOWorker::stop()
{
    running = false;
    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void IOWorker::performIO()
{
    // Use detected sector size for alignment
    const uint32_t alignment = std::max(sectorSize, static_cast<uint32_t>(4096));
    const size_t alignedSize = ((config.requestKB * 1024 + alignment - 1) / alignment) * alignment;

    void *alignedBuf = nullptr;
    if (posix_memalign(&alignedBuf, alignment, alignedSize) != 0)
    {
        throw std::runtime_error("Failed to allocate aligned buffer");
    }

    try
    {
        // Initialize buffer for writes
        memset(alignedBuf, 0xFF, alignedSize);

        while (running)
        {
            uint64_t offset = getNextOffset();
            // Align offset to sector size
            offset = (offset + alignment - 1) & ~(static_cast<uint64_t>(alignment) - 1);

            if (offset + alignedSize > static_cast<uint64_t>(config.rangeMB) * 1024 * 1024)
            {
                currentOffset = 0;
                continue;
            }

            double startTime = Utils::getCurrentTime();
            ssize_t result;

            if (config.isRead)
            {
                result = pread(fd, alignedBuf, alignedSize, static_cast<off_t>(offset));
            }
            else
            {
                result = pwrite(fd, alignedBuf, alignedSize, static_cast<off_t>(offset));
            }

            if (result < 0)
            {
                std::cerr << "I/O error in thread " << threadId << ": "
                          << strerror(errno) << " at offset " << offset
                          << " size " << alignedSize << std::endl;
                continue;
            }
            else if (result > 0)
            {
                double endTime = Utils::getCurrentTime();
                double latencyMs = (endTime - startTime) * 1000.0;

                stats.recordRequest(threadId,
                                    config.isRead,
                                    offset,
                                    static_cast<uint64_t>(result),
                                    latencyMs);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Thread " << threadId << " error: " << e.what() << std::endl;
    }

    free(alignedBuf);
}

uint64_t IOWorker::getNextOffset()
{
    if (config.isSequential)
    {
        uint64_t offset = currentOffset;
        uint64_t maxOffset = static_cast<uint64_t>(config.rangeMB) * 1024 * 1024;
        uint64_t alignedRequestSize = ((config.requestKB * 1024 + sectorSize - 1) / sectorSize) * sectorSize;
        currentOffset += alignedRequestSize;

        if (currentOffset >= maxOffset)
        {
            currentOffset = 0;
        }

        return offset;
    }
    else
    {
        uint64_t maxOffset = static_cast<uint64_t>(config.rangeMB) * 1024 * 1024;
        uint64_t alignedRequestSize = ((config.requestKB * 1024 + sectorSize - 1) / sectorSize) * sectorSize;
        uint64_t randomOffset = Utils::getRandomOffset(maxOffset, alignedRequestSize);
        return randomOffset & ~(static_cast<uint64_t>(sectorSize) - 1);
    }
}