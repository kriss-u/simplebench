#include "ArgumentParser.h"
#include <getopt.h>
#include <iostream>

void ArgumentParser::printUsage()
{
    std::cout << "Usage: simplebench [OPTIONS]\n"
              << "Options:\n"
              << "  -h              Show this help message\n"
              << "  -e <Duration>   Test duration in seconds (1-3600)\n"
              << "  -f <Device>     Target device or file path\n"
              << "  -r <Range>      Accessible range in MB (1-1048576)\n"
              << "  -s <Size>       Request size in KB (1-512)\n"
              << "  -t <Type>       Request type: R (Read) or W (Write)\n"
              << "  -p <Pattern>    Access pattern: S (Sequential) or R (Random)\n"
              << "  -q <Threads>    Number of parallel threads (1-32)\n"
              << "  -d <Direct>     Use direct I/O: T (True) or F (False)\n"
              << "  -o <Output>     Output trace file path\n\n"
              << "Example:\n"
              << "  simplebench -e 60 -f /dev/sdb -r 1024 -s 4 -t R -p R -q 2 -d T -o /tmp/test.tr\n";
}

BenchmarkConfig ArgumentParser::parse(int argc, char *argv[])
{
    // Handle help flag before any other processing
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            printUsage();
            exit(0);
        }
    }

    if (argc < 2)
    {
        printUsage();
        throw std::runtime_error("No arguments provided");
    }

    BenchmarkConfig config{}; // Zero-initialize all fields
    int opt;

    while ((opt = getopt(argc, argv, "he:f:r:s:t:p:q:d:o:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printUsage();
            exit(0);
        case 'e':
            config.duration = static_cast<uint32_t>(std::stoul(optarg));
            break;
        case 'f':
            config.devicePath = optarg;
            break;
        case 'r':
            config.rangeMB = static_cast<uint32_t>(std::stoul(optarg));
            break;
        case 's':
            config.requestKB = static_cast<uint32_t>(std::stoul(optarg));
            break;
        case 't':
            config.isRead = (optarg[0] == 'R');
            break;
        case 'p':
            config.isSequential = (optarg[0] == 'S');
            break;
        case 'q':
            config.numThreads = static_cast<uint32_t>(std::stoul(optarg));
            break;
        case 'd':
            config.useDirectIO = (optarg[0] == 'T');
            break;
        case 'o':
            config.outputPath = optarg;
            break;
        case '?':
            printUsage();
            throw std::runtime_error("Invalid arguments");
        default:
            printUsage();
            throw std::runtime_error("Invalid arguments");
        }
    }
    return config;
}