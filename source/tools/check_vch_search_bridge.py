"""Independent strict checks of real c49 Wine observations, not native evidence."""
import json
import sys
from pathlib import Path

rows = [json.loads(line) for line in Path(sys.argv[1]).read_text().splitlines() if line.startswith('{')]
assert len(rows) == 6
assert {(r['mode'], r['scenario']) for r in rows} == {(m, s) for m in ['reset', 'Position'] for s in ['close', 'deadline', 'kill']}
for r in rows:
    assert r['task'] == 'c49' and r['environment'] == 'Wine'
    assert r['result'] == 'VCH_SEARCH_CONTINUATION_BRIDGE_UNAVAILABLE'
    assert r['reason'] == 'registered_VCH_not_dispatched_for_mixed_SEARCH_DB'
    assert r['prerequisites_admitted'] and r['mitigation_classifier']['admitted']
    assert r['WS_disposition'] == 'CONTINUE_SEARCH' and r['downstream_disposition'] == 'CONTINUE_EXECUTION'
    assert r['WS_calls'] == r['downstream_calls'] == 1
    assert r['VCH_registered'] and r['VCH_callback_count'] == 0
    assert r['stages']['VCH'] is None and not r['ordered_final_context_observed'] and not r['native_boundary_observed']
    a, b, c, d = [r['stages'][key] for key in ['WS_entry', 'WS_after_engine', 'downstream_before', 'downstream_final']]
    debt = r['reserved_debt']
    assert a['order'] < b['order'] < debt['reserved_order'] < debt['SEARCH_escape_order'] < c['order'] < d['order']
    assert debt['thread_creation_epoch'] > 0 and debt['lease'] == debt['sequence'] == 1 and debt['generation'] == 149
    for s in [a, b, c, d]:
        assert s['thread'] == debt['thread'] and s['context'] == debt['context'] and s['exception'] == debt['exception']
        assert s['Rip'] == debt['original_Rip'] and s['Dr2'] == a['Dr2']
        assert all(s[key] == a[key] for key in ['Dr0', 'Dr1', 'Dr2', 'Dr3', 'Dr7', 'EFlags', 'ContextFlags'])
    assert int(a['Dr6'], 16) ^ int(b['Dr6'], 16) == 1
    assert b['Dr6'] == c['Dr6'] and int(c['Dr6'], 16) ^ int(d['Dr6'], 16) == 4
    assert int(d['Dr6'], 16) & 0xe00f == 0
    assert r['pending_before_witness'] == r['pending_at_worker_exit'] == 1
    assert r['VCH_to_witness_binding'] is None and r['witness_acknowledgment'] is None and r['pending_after_witness'] is None
    assert not r['can_retire_before_worker_exit'] and not r['live_final_cleanup_performed'] and not r['poisoned']
    assert r['cleanup_requested_while_pending'] and r['cleanup_after_worker_exit']
    assert r['resurrection_after_final_cleanup'] is None and not r['bridge_implemented']
    assert r['observer_Rip_writes'] == r['observer_DR_repairs'] == 0 and r['foreign_DR6_cause_clears'] == 1
    assert not r['production_integrated'] and not r['exact_slot_watch_enabled'] and not r['game_used']
print('PASS: c49 six actual mixed SEARCH/downstream B2-clear observations parsed; callback0 remains UNAVAILABLE, no modeled/native bridge PASS')
