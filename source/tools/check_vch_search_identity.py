"""c50 strict serialization check: synthetic truth and real Wine stay separate."""
import json
import sys
from pathlib import Path

names = ['ws_search_ok', 'entry_cause_expected', 'post_engine_cause_expected', 'cause_delta_expected', 'rip_preserved',
         'Dr0_preserved', 'Dr1_preserved', 'Dr2_preserved', 'Dr3_preserved', 'Dr7_preserved',
         'global_used_leases_is_one', 'legacy_first_own_mask_is_three', 'legacy_window_zero_id_ok', 'legacy_window_zero_generation_ok']
rows = [json.loads(s) for s in Path(sys.argv[1]).read_text().splitlines() if s.startswith('{')]
assert len(rows) == 7
ordinary = [r for r in rows if not r['unrelated_worker_requested']]
assert {(r['mode'], r['scenario']) for r in ordinary} == {(m, s) for m in ['reset', 'Position'] for s in ['close', 'deadline', 'kill']}
for r in rows:
    assert r['task'] == 'c50' and r['schema'] == 2 and r['environment'] == 'Wine'
    assert r['result'] == 'MIXED_SEARCH_IDENTITY_DIAGNOSTIC_UNAVAILABLE' and not r['poisoned']
    assert r['WS_disposition'] == 'CONTINUE_SEARCH' and r['downstream_disposition'] == 'CONTINUE_EXECUTION'
    assert r['VCH_registered'] and r['VCH_callback_count'] == 0 and r['stages']['VCH'] is None
    assert not r['native_boundary_observed'] and not r['ordered_final_context_observed']
    assert r['pending_at_worker_exit'] == r['pending_before_witness'] == 1 and r['cleanup_after_worker_exit']
    assert r['witness_acknowledgment'] is None and r['pending_after_witness'] is None and r['VCH_to_witness_binding'] is None
    assert not r['bridge_implemented'] and not r['live_final_cleanup_performed'] and not r['observer_Rip_writes'] and not r['observer_DR_repairs']
    assert all(not r[k] for k in ['production_integrated', 'exact_slot_watch_enabled', 'game_used'])
    d = r['identity_diagnostics']
    assert all(type(d[k]) is bool for k in names)
    assert d['entry_cause_mask'] == 5 and d['post_engine_cause_mask'] == 4 and d['cause_delta'] == 1
    assert d['matching_worker_lease_count'] == d['matching_owned_context_lease_count'] == 1
    assert d['selected_window_id'] == d['expected_window_id'] == 1
    assert d['selected_window_generation'] == d['expected_generation'] == 149 and d['selected_own_mask'] == 3
    assert d['snapshot_complete_and_stable'] and d['UNIQUE_MATCHING_DEBT_IDENTITY'] and d['c50_diagnostic_admissible']
    assert len(d['leases']) == d['records_observed'] == d['used_leases_total']
    selected = d['leases'][d['selected_lease_index']]
    assert selected['tid'] == r['reserved_debt']['thread'] and selected['own_mask'] == 3 and selected['installed'] and not selected['suspended']
    assert all(l['metadata_known'] and l['status_known'] for l in d['leases'])
    if r['unrelated_worker_requested']:
        assert d['used_leases_total'] == d['context_owning_lease_count_all_threads'] == 2
        assert not d['legacy_c49_aggregate'] and d['only_global_usedLeases_conjunct_failed']
        assert d['legacy_failed_conjuncts'] == ['global_used_leases_is_one']
    else:
        assert d['used_leases_total'] == 1 and d['legacy_c49_aggregate'] and not d['legacy_failed_conjuncts']
    assert all(d[k] for k in names if k != 'global_used_leases_is_one')
    debt = r['reserved_debt'];a, b, c, f = [r['stages'][k] for k in ['WS_entry', 'WS_after_engine', 'downstream_before', 'downstream_final']]
    assert a['order'] < b['order'] < debt['reserved_order'] < debt['SEARCH_escape_order'] < c['order'] < f['order']
    for stage in [a, b, c, f]:
        assert stage['context'] == debt['context'] and stage['exception'] == debt['exception'] and stage['thread'] == debt['thread'] and stage['Rip'] == debt['original_Rip']
    assert int(a['Dr6'], 16) ^ int(b['Dr6'], 16) == 1 and b['Dr6'] == c['Dr6'] and int(c['Dr6'], 16) ^ int(f['Dr6'], 16) == 4
synthetic = json.loads(Path(sys.argv[2]).read_text())
assert synthetic['synthetic_identity_fixture'] and not synthetic['native_execution']
d = synthetic['evidence'];assert not d['legacy_c49_aggregate'] and d['UNIQUE_MATCHING_DEBT_IDENTITY']
assert d['legacy_failed_conjuncts'] == ['global_used_leases_is_one'] and d['selected_lease_index'] == 1
assert set(names) <= d.keys()
print('PASS: c50 all14 individual conjuncts/lease summaries parsed; actual unrelated-thread global2 vs unique1 stays Wine UNAVAILABLE; synthetic truth is separate')
