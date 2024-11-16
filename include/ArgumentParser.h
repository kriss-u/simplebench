#pragma once
#include <string>
#include <stdexcept>
#include <cstdint>
#include "BenchmarkConfig.h"

namespace ArgumentParser
{
    BenchmarkConfig parse(int argc, char *argv[]);
    void printUsage();
}