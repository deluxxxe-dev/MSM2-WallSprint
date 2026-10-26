#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/pre_access_lifecycle_tests.cpp -o build/pre_access_lifecycle_tests
./build/pre_access_lifecycle_tests | tee logs/pre_access_lifecycle_tests.log
./build/pre_access_lifecycle_tests --json-fixture > logs/pre_access_lifecycle_synthetic.json
python3 - <<'PY' | tee -a logs/pre_access_lifecycle_tests.log
import json
from pathlib import Path
r=json.loads(Path('logs/pre_access_lifecycle_synthetic.json').read_text())
assert r['synthetic_pre_access_fixture'] is True
for key,label in [('pre_access','WATCH_ENABLED_RESUME_PRE_ACCESS'),('delivery','DEBUG_EXCEPTION_DELIVERY_BOUNDARY')]:
    x=r[key]
    assert x['revision']=='c46-v1' and x['gate_proven'] and x['complete_ordered']
    assert x['RETIREMENT_CONFIGURATION_SAFE_added'] is False
    assert len(x['stages'])==12 and x['stages'][4]['phase']=='post_watch_resume_pre_access'
    for name in ['DB_events_at_arrival','DB_events_before_suspend','DB_events_at_capture','DB_events_after_read','machine_entries_at_capture','tickets_at_capture','pending_at_capture']:
        assert x['gate'][name]==0
    for bit in ['bit11','bit16']:
        assert x[bit]['classification']==label and x[bit]['changes_observed']==1 and x[bit]['unique']
    assert [s['sequence'] for s in x['stages']]==list(range(1,13))
print('PASS: c46 strict synthetic JSON keeps12 ordered phases and zero-DB gate evidence separate from absent configuration acceptance')
PY
