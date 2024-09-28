#!/bin/bash
set -e

cmake -B build
cmake --build build
#ctest --test-dir build --output-on-failure

./build/backend/backend
