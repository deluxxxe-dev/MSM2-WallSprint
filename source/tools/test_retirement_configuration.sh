#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/retirement_configuration_tests.cpp -o build/retirement_configuration_tests
./build/retirement_configuration_tests | tee logs/retirement_configuration_tests.log
./build/retirement_configuration_tests --json-fixture > logs/retirement_configuration_synthetic.json
python3 - <<'PY' | tee -a logs/retirement_configuration_tests.log
import json
from pathlib import Path
x=json.loads(Path('logs/retirement_configuration_synthetic.json').read_text())
assert x['synthetic_configuration_fixture'] is True and x['native_execution'] is False
assert x['RAW_EXACT'] is False and x['ARCHITECTURAL_CANONICAL'] is False
m=x['model'];assert m['revision']=='c47-v1' and m['synthetic_inputs'] is True
assert m['RETIREMENT_CONFIGURATION_SAFE'] is True and m['ticket_capture_complete'] and m['ticket_count']==3
assert all(t['acknowledged'] and t['current_code'] for t in m['tickets'])
assert m['total_context_write_attempts']==2 and m['final_poll_context_write_attempts']==1
assert m['failed_context_write_attempts']==m['unscoped_context_write_attempts']==0
print('PASS: c47 synthetic JSON independently preserves false raw/c44 and true scoped model without claiming native execution')
PY
