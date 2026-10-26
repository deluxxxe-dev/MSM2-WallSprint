"""Validate real owned Wine results; synthetic DWORD/unit fixtures are separate."""
import json
import sys
from pathlib import Path

records = [json.loads(line) for line in Path(sys.argv[1]).read_text().splitlines()
           if line.startswith('{')]
assert len(records) == 12
seen = set()
for record in records:
    assert record['schema'] == 3 and record['task'] == 'c43'
    assert record['witness_task'] == 'c41' and record['environment'] == 'Wine'
    assert record['thread'] > 0 and record['lease'] == 1 and record['generation'] == 101
    assert record['game_used'] is False and record['production_integrated'] is False
    assert record['native_candidate_probe_passed'] is False
    assert record['mitigation_classifier']['revision'] == 'c42-v1'
    assert record['mitigation_classifier']['admitted'] is True
    assert [p['flags'] for p in record['mitigation_classifier']['policies']] == record['mitigation_flags']
    mode, scenario = record['mode'], record['scenario']
    assert mode in ['reset', 'Position']
    assert scenario in ['close', 'timeout', 'kill', 'nested', 'multiple', 'cap_refusal']
    assert (mode, scenario) not in seen
    seen.add((mode, scenario))
    assert record['contexts'] == (3 if scenario == 'multiple' else 2 if scenario == 'nested' else 1)
    report = record['final_restoration']
    assert report['revision'] == 'c43-v1' and report['comparison'] == 'EXACT_UNMASKED'
    assert report['synthetic_inputs'] is False
    assert set(report['registers']) == {'Dr0', 'Dr1', 'Dr2', 'Dr3', 'Dr6', 'Dr7'}
    if scenario == 'cap_refusal':
        assert record['result'] == 'FAIL_CLOSED_EXPECTED' and record['retirement_accepted'] is False
        assert report['attempted'] is False and report['capture_complete'] is False
        assert report['classification'] == 'NOT_OBSERVED' and report['final_predicate'] is None
        assert report['active_leases_after_final_poll'] is None and report['final_poll_returned'] is None
        continue
    assert record['result'] == 'OWNED_WITNESS_PASS' and record['retirement_accepted'] is True
    assert report['attempted'] is True and report['capture_complete'] is True
    assert report['active_leases_before_final_poll'] is True
    assert report['active_leases_after_final_poll'] is False
    assert report['final_poll_returned'] is False and report['can_retire_before_final_poll'] is True
    assert report['pending_contexts_before_final_poll'] == report['pending_contexts_after_final_poll'] == 0
    assert report['classification'] == 'ALL_RESTORED' and report['final_predicate'] is True
    for fields in report['registers'].values():
        saved, before, after = (int(fields[k], 16) for k in ['saved', 'before', 'after'])
        assert fields['before_equal'] == (saved == before)
        assert fields['after_equal'] is True and saved == after
        assert int(fields['after_xor'], 16) == saved ^ after
    assert len(report['verification_readbacks']) == 3
    for check in report['verification_readbacks']:
        assert check['captured'] is True and check['same_saved'] is True
        assert all(value == report['registers'][name]['saved'] for name, value in check['registers'].items())
print('PASS: c43 actual Wine diagnostic JSON validates10 exact restorations,2 unobserved cap refusals, before/after boundaries and immutable identity')
