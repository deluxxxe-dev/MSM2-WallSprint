#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/wall_motion_window_tests.cpp -o build/wall_motion_window_tests
./build/wall_motion_window_tests | tee logs/wall_motion_window_tests.log
