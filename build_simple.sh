#!/bin/bash

# Educational Ray Tracer - Simple Build Script
# Single command setup for cross-platform development

echo "=== Educational Ray Tracer Build System ==="
echo "Starting build process with educational visibility..."

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure CMake with educational visibility
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with visible compilation steps
echo "Building project..."
make

# Run tests to verify mathematical correctness
echo "Running mathematical correctness tests..."
./test_math_correctness

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ BUILD SUCCESSFUL"
    echo "Ray tracer executable: ./raytracer"
    echo "Test executable: ./test_math_correctness"
    echo ""
    echo "Ready for Epic 1 development!"
else
    echo ""
    echo "❌ BUILD FAILED - Tests did not pass"
    exit 1
fi