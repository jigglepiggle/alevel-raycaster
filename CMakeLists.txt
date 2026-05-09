# Minimum CMake version required to support the features used below.
cmake_minimum_required(VERSION 3.16)

# Project name, version and language declaration.
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# Require C++17 and do not fall back to an older standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Write compile_commands.json so clangd can provide IDE-style completions.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug if the caller did not specify a build type.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Convenience variables for the source and include directories.
set(SRCDIR src)
set(INCDIR include)

# Collect every .cpp file under src/ automatically.
file(GLOB_RECURSE SOURCES "${SRCDIR}/*.cpp")

# Declare the single executable target built from all discovered source files.
add_executable(main ${SOURCES})

# Make the include/ directory visible to the compiler for header resolution.
target_include_directories(main PRIVATE ${INCDIR})

# Compiler flags: add debug info and warnings in Debug mode; optimise in Release.
target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-g -Wall>
    $<$<CONFIG:Release>:-O2>
)

# Additional warnings enabled per compiler family.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(main PRIVATE -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(main PRIVATE -Wextra)
endif()

# Locate and link the SDL3 library; the build fails if it is not found.
find_package(SDL3 REQUIRED)
target_link_libraries(main PRIVATE SDL3::SDL3)

# Place the compiled binary directly in the build directory.
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Print useful diagnostics during the configure step.
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Sources: ${SOURCES}")

# Custom target that deletes the entire build directory for a full clean.
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
)
