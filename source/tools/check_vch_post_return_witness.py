"""Check actual Wine availability separately from explicit synthetic contracts."""
import json
import sys
from pathlib import Path

old = Path('src/NativeHooks/continuation_witness_smoke.cpp').read_text()
old = old[old.index('bool relative('):old.index('bool canRetire()')]
new = Path('include/NativeHooks/c41_owned_stub.hpp').read_text()
new = new[new.index('inline bool relative('):new.index('} // namespace')].replace('inline ', '')
assert old == new, 'Reviewed c41 helper function bodies must stay unchanged'
data = Path(sys.argv[1]).read_text()
rows = [json.loads(s) for s in data.splitlines() if s.startswith('{')]
assert len(rows) == 2 and {r['unrelated_worker_requested'] for r in rows} == {False, True}
assert data.count('PASS: c51 actual reviewed c41 helper') == 1
for r in rows:
    assert r['task'] == 'c51' and r['schema'] == 3 and r['environment'] == 'Wine'
    assert r['result'] == 'VCH_POST_RETURN_WITNESS_BRIDGE_UNAVAILABLE' and not r['poisoned']
    assert r['WS_disposition'] == 'CONTINUE_SEARCH' and r['downstream_disposition'] == 'CONTINUE_EXECUTION'
    assert r['VCH_registered'] and r['VCH_callback_count'] == 0 and r['stages']['VCH'] is None
    assert r['identity_diagnostics']['UNIQUE_MATCHING_DEBT_IDENTITY']
    assert r['identity_diagnostics']['used_leases_total'] == (2 if r['unrelated_worker_requested'] else 1)
    assert r['pending_before_witness'] == r['pending_at_worker_exit'] == 1 and r['pending_after_witness'] is None
    assert not r['DEBT_ACKNOWLEDGED'] and not r['live_final_cleanup_performed']
    assert r['cleanup_requested_before_witness'] and r['cleanup_after_worker_exit']
    assert r['VCH_context_fields_changed'] == [] and r['VCH_RIP_only'] is None and not r['redirection_exercised']
    assert r['VCH_to_witness_binding'] is None and r['witness_token']['observed'] is None
    assert not r['stub']['prepared'] and not r['stub']['published'] and r['stub']['c41_helpers_unchanged']
    assert r['STUB_EXECUTION_QUIESCENT'] is None and r['STORAGE_RECLAIMABLE'] is None
    assert r['same_event_configuration_restored'] is None and r['RAW_EXACT'] is None and r['ARCHITECTURAL_CANONICAL'] is None
    assert r['resurrection_after_final_cleanup'] is None and r['second_watched_accesses'] == 0 and r['machine_state_preserved']
    assert r['bridge_implemented'] and not r['c47_profile_applied']
    assert r['BLOCKED'] == 0 and all(not r[k] for k in ['production_integrated', 'exact_slot_watch_enabled', 'game_used'])
    d = r['reserved_debt'];s = r['stages']
    assert s['WS_entry']['order'] < s['WS_after_engine']['order'] < r['reservation_order'] < r['SEARCH_escape_order'] < s['downstream_before']['order'] < s['downstream_final']['order']
    assert d['epoch'] and d['lease'] == d['window'] == d['sequence'] == 1 and d['generation'] == 149
fixture = json.loads(Path(sys.argv[2]).read_text())
assert fixture['synthetic_bridge_contract'] and not fixture['native_execution']
assert fixture['pending_after_model_ack'] == 0 and fixture['model_configuration_restored'] and not fixture['live_storage_reclaimable']
assert not fixture['RAW_EXACT'] and not fixture['ARCHITECTURAL_CANONICAL']
print('PASS: c51 reviewed c41 helper bodies identical; actual mixed-machine Wine callback0 stays UNAVAILABLE, synthetic acknowledgment/configuration is not native proof')
