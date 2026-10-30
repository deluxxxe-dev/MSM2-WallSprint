#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/reversal_heading_tests.cpp -o build/reversal_heading_tests
./build/reversal_heading_tests | tee logs/reversal_heading_tests.log
