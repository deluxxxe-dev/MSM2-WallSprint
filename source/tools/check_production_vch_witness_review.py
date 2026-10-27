"""c52 static audit integrity; never a model/native/production acceptance test."""
import hashlib
import json
import re
from pathlib import Path

root = Path(__file__).resolve().parents[2]
analysis = root / 'analysis'
status = json.loads((analysis / 'V01C52_STATUS.json').read_text())

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def passed(message):
    print('PASS: c52 static audit: ' + message)

old = json.loads((analysis / 'native-probes/c51/PROBE_MANIFEST.json').read_text())
inputs = [x for x in old['source_inputs'] if x['path'] != 'source/tools/package.py']
assert len(inputs) == 272
for item in inputs:
    assert digest(root / item['path']) == item['sha256'], item['path']
old_code = {x['path'] for x in inputs}
for directory in ['src', 'include']:
    for path in (root / 'source' / directory).rglob('*'):
        if path.is_file():
            assert path.relative_to(root).as_posix() in old_code, str(path)
passed('all272 prior code/tool inputs unchanged; no new production/helper code')

for item in status['historical_inputs']:
    assert digest(root / item['path']) == item['sha256'], item['path']
passed('c39-c51 historical proof, native probes and task input byte identities preserved')

source = (root / 'source/src/msm2/orientation_trace.cpp').read_text()
lines = source.splitlines()
handler = source[source.index('LONG CALLBACK handler('):source.index('bool resume(')]
assert len(re.findall(r'\breturn\b[^;]*EXCEPTION_CONTINUE_', handler)) == 5
rows = status['return_paths']
assert {x['id'] for x in rows} == {'S0', 'S1', 'S2', 'S3', 'T0'} | {'C' + str(i) for i in range(9)}
for row in rows:
    segment = '\n'.join(lines[row['begin_line'] - 1:row['end_line']]) + '\n'
    assert hashlib.sha256(segment.encode()).hexdigest() == row['span_sha256']
    assert all(row[k] for k in ['disposition', 'mutation', 'identity_obligations',
                               'reservation', 'final_VCH_target', 'lifetime'])
passed('five textual returns and all14 S/C/terminal inventory spans match audited source')

native = (analysis / 'V01C51_NATIVE_WINDOWS_RESULT.md').read_text()
assert digest(analysis / 'V01C51_NATIVE_WINDOWS_RESULT.md') == status['accepted_native_c51_sha256']
assert 'VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_PASS' in native
assert 'storage_freed_after_exit = true' in native
assert 'no debug-register repair occurred' in native
assert old['native_c51_executed'] is False
assert old['primary_result'] == 'VCH_POST_RETURN_WITNESS_BRIDGE_UNAVAILABLE'
assert status['accepted_native_c51_result'] == 'VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_PASS'
passed('accepted c51 native PASS is distinct from immutable historical Wine UNAVAILABLE')

docs = status['official_documentation']
assert len(docs) == 11 and all(x['available'] for x in docs)
assert len({x['id'] for x in docs}) == 11
for item in docs:
    assert item['url'].startswith('https://raw.githubusercontent.com/MicrosoftDocs/')
    assert re.fullmatch('[0-9a-f]{64}', item['raw_sha256'])
    assert item['contract_excerpt'] and item['review_limit']
assert status['supported_terminal_VCH_boundary_established'] is False
passed('11 official-document provenance records retained; no unsupported terminal-order claim')

assert set(status['ledger_states']) == {'FREE', 'RESERVED', 'PUBLISHED',
    'FINAL_CONTEXT_BOUND', 'RIP_REDIRECTED', 'DEBT_ACKNOWLEDGED',
    'EXECUTION_QUIESCENT_OR_THREAD_EXIT', 'RECLAIMABLE', 'UNKNOWN_POISON'}
assert not status['positive_production_shape_harness_implemented']
assert not status['bounded_no_drop_ledger_established']
assert status['primary_result'] == 'PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert status['owner_action'] == 'none_review_only'
passed('necessary ledger states/refusal boundaries recorded; no positive design/model certificate')

package = (root / 'source/tools/package.py').read_text()
assert "manifest['v01c51_gate']='VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_PASS'" in package
assert "manifest['v01c52_gate']='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'" in package
assert 'V01C51_NATIVE_WINDOWS_RESULT.md' in package and 'V01C52_STATUS.json' in package
assert 'production_vch_witness' not in (root / 'source/tools/build_windows.sh').read_text()
assert not (analysis / 'native-probes/c52').exists()
assert not any(status[k] for k in ['production_changed', 'exact_slot_watch_enabled', 'game_used', 'new_game_release'])
passed('package separates accepted fixture PASS from review FAIL; production/native-artifact scope closed')

for name in ['V01C52_PRODUCTION_VCH_WITNESS_REVIEW.md', 'V01C52_GATE.md', 'V01C52_STATUS.json',
             'V01C51_NATIVE_WINDOWS_RESULT.md']:
    assert (analysis / name).read_bytes() == (root / 'source/analysis' / name).read_bytes()
passed('root/source audit and accepted-native evidence mirrors agree')
