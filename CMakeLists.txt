# Minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Project name and version
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable debug symbols and warnings (equivalent to your -g -Wall flags)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Define directories
set(SRCDIR src)
set(INCDIR include)

# Find all source files in the src directory
file(GLOB_RECURSE SOURCES "${SRCDIR}/*.cpp")

# Create the executable target
add_executable(main ${SOURCES})

# Add include directories
target_include_directories(main PRIVATE ${INCDIR})

# SDL3 approach using find_package
find_package(SDL3 REQUIRED)
target_link_libraries(main PRIVATE SDL3::SDL3)

# Optional: Set output directory to match your Makefile structure
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Print some useful information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Source files found: ${SOURCES}")

# Custom target for cleaning (equivalent to your clean target)
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing build directory"
)
