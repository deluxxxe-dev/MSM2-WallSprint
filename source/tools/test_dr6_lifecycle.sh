#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/dr6_lifecycle_tests.cpp -o build/dr6_lifecycle_tests
./build/dr6_lifecycle_tests | tee logs/dr6_lifecycle_tests.log
./build/dr6_lifecycle_tests --json-fixture > logs/dr6_lifecycle_synthetic.json
python3 - <<'PY' | tee -a logs/dr6_lifecycle_tests.log
import json
from pathlib import Path
d=json.loads(Path('logs/dr6_lifecycle_synthetic.json').read_text())
assert d['synthetic_lifecycle_fixture'] is True
a,b=d['unique'],d['multiple']
assert a['complete_ordered'] and a['necessary_conditions_only'] and not a['RETIREMENT_STATUS_SAFE_added']
assert not b['necessary_conditions_only'] and not b['RETIREMENT_STATUS_SAFE_added']
for bit in ['bit11','bit16']:
    assert a[bit]['classification']=='HARDWARE_DB_ENTRY' and a[bit]['changes_observed']==1
    assert b[bit]['classification']=='UNRESOLVED_OR_MULTIPLE' and b[bit]['changes_observed']==3
for item in [a,b]:
    assert len(item['stages'])==11
    for stage in item['stages']:
        value=int(stage['registers']['Dr6'],16)
        assert stage['Dr6_bit11']==(value>>11)&1 and stage['Dr6_bit16']==(value>>16)&1
print('PASS: c45 strict synthetic JSON preserves11 ordered stages, explicit per-bit transitions and necessary-only non-certificate scope')
PY
