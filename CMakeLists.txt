cmake_minimum_required(VERSION 3.10)
project(simplebench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(simplebench ${SOURCES})

# Link against required libraries
target_link_libraries(simplebench pthread)