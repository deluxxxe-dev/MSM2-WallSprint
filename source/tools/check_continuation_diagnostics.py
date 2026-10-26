"""Validate real owned Wine results; synthetic DWORD/unit fixtures are separate."""
import json
import sys
from pathlib import Path

records = [json.loads(line) for line in Path(sys.argv[1]).read_text().splitlines()
           if line.startswith('{')]
assert len(records) == 12
seen = set()
for record in records:
    assert record['schema'] == 4 and record['task'] == 'c44'
    assert record['witness_task'] == 'c41' and record['environment'] == 'Wine'
    assert record['thread'] > 0 and record['lease'] == 1 and record['generation'] == 101
    assert record['game_used'] is False and record['production_integrated'] is False
    assert record['native_candidate_probe_passed'] is False
    assert record['cpu_vendor'] in ['GenuineIntel', 'AuthenticAMD'] and record['cpuid_known'] is True
    assert record['acceptance_comparison'] == 'RAW_EXACT'
    assert record['acceptance_scope'] == 'owned_Wine_only_not_architectural_certificate'
    comparison = record['debug_register_comparison']
    assert comparison['revision'] == 'c44-v1' and comparison['cpu_vendor'] == record['cpu_vendor']
    for masks in comparison['masks'].values():
        total = 0
        for value in masks.values():
            mask = int(value,16)
            assert total & mask == 0
            total |= mask
        assert total == (1<<64)-1
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
        assert comparison['readbacks_complete'] is False and comparison['after'] is None
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
    assert comparison['readbacks_complete'] is True and comparison['raw_all_exact'] is True
    assert comparison['raw_readbacks_stable'] is True and comparison['architectural_results_stable'] is True
    for phase, snapshot in zip([comparison['after']]+comparison['verification_readbacks'],[report['registers']]+[r['registers'] for r in report['verification_readbacks']]):
        assert phase['RAW_EXACT'] is True and phase['Dr0_Dr3_exact'] is True
        assert phase['meaningful_exact'] is True and phase['OS_controlled_exact'] is True and phase['unknown_bits_clear'] is True
        valid = True
        for reg in ['Dr6','Dr7']:
            value = snapshot[reg]['after'] if isinstance(snapshot[reg],dict) else snapshot[reg]
            current = int(value,16);m = comparison['masks'][reg]
            one, zero = int(m['FIXED_ONE'],16), int(m['FIXED_ZERO'],16)
            valid &= current & one == one and current & zero == 0
        current_dr6 = snapshot['Dr6']['after'] if isinstance(snapshot['Dr6'],dict) else snapshot['Dr6']
        boundary = int(current_dr6,16) & 0x10800 == 0x10800
        assert phase['status_boundary_valid'] == boundary
        assert phase['current_fixed_bits_valid'] == valid and phase['ARCHITECTURAL_CANONICAL'] == (valid and boundary)
print('PASS: c44 actual Wine JSON validates10 RAW_EXACT restorations,2 cap refusals, CPUID vendor,128-bit masks and three separate architectural comparisons without native claims')
