cmake_minimum_required(VERSION 3.20)
project(raytracer VERSION 1.0.0 LANGUAGES CXX)

# Set C++20/C++23 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Educational build visibility - show compilation commands
set(CMAKE_VERBOSE_MAKEFILE ON)

# Platform-specific optimizations
if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    # Apple Silicon optimization flags
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mcpu=apple-m1 -DNDEBUG")
    message(STATUS "Apple Silicon detected - using optimized flags: ${CMAKE_CXX_FLAGS_RELEASE}")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    # Intel/AMD optimization
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
    message(STATUS "Intel/AMD processor detected - using optimized flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# Cross-platform compatibility preprocessor directives
if(APPLE)
    add_compile_definitions(PLATFORM_APPLE)
elseif(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
elseif(UNIX)
    add_compile_definitions(PLATFORM_LINUX)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(raytracer ${SOURCES})

# Enable testing
enable_testing()

# Test executable
add_executable(test_math_correctness tests/test_math_correctness.cpp)
add_test(NAME MathematicalTests COMMAND test_math_correctness)

# Educational build information
message(STATUS "=== Educational Ray Tracer Build Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "==================================================")