# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -Iinclude
LIBS = -pthread

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = simplebench

# Default target
all: $(BUILD_DIR)/$(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Link object files
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean