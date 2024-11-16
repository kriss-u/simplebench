#pragma once
#include <string>
#include <stdexcept>
#include <cstdint>
#include "BenchmarkConfig.h"

class ArgumentParser
{
public:
    // Prevent instantiation
    ArgumentParser() = delete;
    ArgumentParser(const ArgumentParser &) = delete;
    ArgumentParser &operator=(const ArgumentParser &) = delete;

    // Main parsing function
    static BenchmarkConfig parse(int argc, char *argv[]);

    // Print usage information
    static void printUsage();
};