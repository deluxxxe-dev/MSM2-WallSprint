#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/debug_register_comparison_tests.cpp -o build/debug_register_comparison_tests
./build/debug_register_comparison_tests | tee logs/debug_register_comparison_tests.log
./build/debug_register_comparison_tests --json-fixture > logs/debug_register_comparison_synthetic.json
python3 - <<'PY' | tee -a logs/debug_register_comparison_tests.log
import json
from pathlib import Path
d=json.loads(Path('logs/debug_register_comparison_synthetic.json').read_text())
assert d['synthetic_comparator_fixture'] is True
assert d['fixed_only']['RAW_EXACT'] is False and d['fixed_only']['ARCHITECTURAL_CANONICAL'] is True
for k in ['observed_c43_Intel','observed_c43_AMD']:
    assert d[k]['RAW_EXACT'] is False and d[k]['ARCHITECTURAL_CANONICAL'] is False
assert d['unknown_vendor']['ARCHITECTURAL_CANONICAL'] is False
print('PASS: c44 strict synthetic JSON round-trip separates RAW_EXACT and ARCHITECTURAL_CANONICAL without creating native evidence')
PY
