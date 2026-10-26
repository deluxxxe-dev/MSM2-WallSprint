#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/continuation_diagnostics_tests.cpp -o build/continuation_diagnostics_tests
./build/continuation_diagnostics_tests | tee logs/continuation_diagnostics_tests.log
./build/continuation_diagnostics_tests --json-fixture > logs/continuation_diagnostics_synthetic.json
python3 - <<'PY' | tee -a logs/continuation_diagnostics_tests.log
import json
from pathlib import Path
d=json.loads(Path('logs/continuation_diagnostics_synthetic.json').read_text())
assert d['synthetic_diagnostic_fixture'] is True
for r,kind in zip(d['reports'],['ALL_RESTORED','LEASE_ONLY_FAILURE','DR_ONLY_FAILURE','LEASE_AND_DR_FAILURE','NOT_OBSERVED']):
    assert r['synthetic_inputs'] is True and r['classification']==kind
    assert r['final_predicate'] is (True if kind=='ALL_RESTORED' else None if kind=='NOT_OBSERVED' else False)
    assert r['capture_complete'] is (kind!='NOT_OBSERVED')
    for fields in r['registers'].values():
        if kind=='NOT_OBSERVED':
            assert all(v is None for v in fields.values())
        else:
            saved,before,after=(int(fields[k],16) for k in ['saved','before','after'])
            assert fields['before_equal']==(saved==before) and fields['after_equal']==(saved==after)
            assert int(fields['after_xor'],16)==saved^after
print('PASS: c43 strict synthetic JSON round-trip preserves all four outcomes, null observations, exact values and XOR')
PY
