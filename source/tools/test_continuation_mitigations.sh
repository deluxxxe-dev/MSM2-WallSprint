#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/continuation_mitigations_tests.cpp -o build/continuation_mitigations_tests
./build/continuation_mitigations_tests | tee logs/continuation_mitigations_tests.log
./build/continuation_mitigations_tests --json-fixture > logs/continuation_mitigations_synthetic.json
python3 - <<'PY' | tee -a logs/continuation_mitigations_tests.log
import json
from pathlib import Path
d=json.loads(Path('logs/continuation_mitigations_synthetic.json').read_text())
assert d['synthetic_classifier_fixture'] is True
a,b,c=d['reports']
assert a['admitted'] and not b['admitted'] and not c['admitted']
p=a['policies'][2]
assert p['flags']==256 and p['irrelevant_mask']==256 and p['enabled_bits'][0]['name']=='CetDynamicApisOutOfProcOnly'
assert p['enabled_bits'][0]['classification']=='IRRELEVANT_TO_C41_CANDIDATE'
assert b['policies'][2]['unknown_refuse_mask']==2147483648 and b['policies'][2]['relevant_refuse_mask']==1
assert c['policies'][2]['flags']==256 and c['policies'][2]['decision']=='QUERY_UNKNOWN_REFUSE' and not c['policies'][2]['enabled_bits']
print('PASS: c42 strict JSON parser round-trip of explicitly synthetic admitted, mixed-protection/high-bit and query-unknown reports; no runtime API output forged')
PY
