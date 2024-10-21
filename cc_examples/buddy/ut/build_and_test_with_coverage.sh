#!/bin/bash

# Exit on error
set -e

# Build the project with coverage flags
cd build
cmake ..
make

# Run test suite
./BuddyAllocatorTest

# Capture coverage data and generate reports
echo "Generating coverage reports..."
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '*/usr/*' '*/ut/*' '*/gtest/*' --output-file coverage_filtered.info
lcov --list coverage_filtered.info

# convert lcov to cobertura
lcov_cobertura coverage_filtered.info

# genhtml coverage_filtered.info --output-directory coverage_report
# echo "Coverage report generated at $(pwd)/coverage_report/index.html"
