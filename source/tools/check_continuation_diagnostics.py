"""Validate real owned Wine results; synthetic DWORD/unit fixtures are separate."""
import json
import sys
from pathlib import Path

records = [json.loads(line) for line in Path(sys.argv[1]).read_text().splitlines()
           if line.startswith('{')]
assert len(records) == 12
seen = set()
for record in records:
    assert record['schema'] == 7 and record['task'] == 'c47'
    assert record['witness_task'] == 'c41' and record['environment'] == 'Wine'
    assert record['thread'] > 0 and record['lease'] == 1 and record['generation'] == 101
    assert record['game_used'] is False and record['production_integrated'] is False
    assert record['native_candidate_probe_passed'] is False
    assert record['cpu_vendor'] == 'GenuineIntel' and record['cpuid_known'] is True
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
    life = record['dr6_lifecycle']
    assert life['revision'] == 'c45-v1' and life['worker'] == record['thread']
    assert life['RETIREMENT_STATUS_SAFE_added'] is False and life['necessary_conditions_only'] is False
    assert life['observer_error'] is False and life['ordinary_owned_data_DB'] is True and life['engine_claimed'] is True
    assert record['watch_context_writes_observed'] == record['outer_hardware_DB_events'] == 1
    assert record['observer_win32_error'] == 0
    phases = ['baseline_pre_foreign','post_foreign_set_readback','post_foreign_resume_saved',
              'post_watch_set_readback_pre_resume','outer_db_handler_entry','outer_db_handler_after_engine',
              'post_return_pre_final_poll','after_final_poll',
              'verification_readback_1','verification_readback_2','verification_readback_3']
    stages = life['stages']
    assert [s['phase'] for s in stages] == phases
    captured = 7 if scenario == 'cap_refusal' else 11
    assert life['complete_ordered'] is (captured == 11)
    for i, stage in enumerate(stages):
        assert stage['captured'] is (i < captured)
        if not stage['captured']:
            continue
        assert stage['sequence'] == i+1+(i>=4) and stage['thread'] == record['thread']
        assert stage['origin'] == ('saved_exception_context' if i in [4,5] else 'suspended_GetThreadContext')
        value = int(stage['registers']['Dr6'],16)
        assert stage['Dr6_bit11'] == (value>>11)&1 and stage['Dr6_bit16'] == (value>>16)&1
        assert int(stage['registers']['Dr7'],16) & 0x800 == 0
    for bit in [11,16]:
        changes = [{'from': phases[i-1], 'to': phases[i], 'before': stages[i-1][f'Dr6_bit{bit}'], 'after': stages[i][f'Dr6_bit{bit}']}
                   for i in range(1,captured) if stages[i-1][f'Dr6_bit{bit}'] != stages[i][f'Dr6_bit{bit}']]
        result = life[f'bit{bit}']
        assert result['transitions'] == changes and result['changes_observed'] == len(changes)
        assert result['first_observed_stage'] == (changes[0]['to'] if changes else None)
        if captured < 11 or len(changes)>1:
            assert result['classification']=='UNRESOLVED_OR_MULTIPLE' and result['unique'] is False
        elif not changes:
            assert result['classification']=='UNCHANGED' and result['unique'] is True
        else:
            expected={'post_foreign_set_readback':'SETTHREADCONTEXT_ROUNDTRIP',
                      'post_watch_set_readback_pre_resume':'SETTHREADCONTEXT_ROUNDTRIP',
                      'outer_db_handler_entry':'HARDWARE_DB_ENTRY','outer_db_handler_after_engine':'HANDLER_EDIT',
                      'post_return_pre_final_poll':'OS_RETURN_POST_CONTEXT'}.get(changes[0]['to'],'UNRESOLVED_OR_MULTIPLE')
            assert result['classification']==expected and result['unique'] is (expected!='UNRESOLVED_OR_MULTIPLE')
    pre = record['pre_access_lifecycle']
    assert pre['revision']=='c46-v1' and pre['gate_proven'] is True and pre['gate_reason']=='none'
    assert pre['RETIREMENT_CONFIGURATION_SAFE_added'] is False and pre['necessary_conditions_only'] is False
    gate = pre['gate']
    for name in ['reached','captured_suspended','captured_before_release','release_unsignaled_at_capture','resumed_after_capture','active_lease','owned_slots']:
        assert gate[name] is True
    assert gate['arrival_thread']==record['thread'] and gate['lease']==record['lease'] and gate['generation']==record['generation']
    for name in ['DB_events_at_arrival','DB_events_before_suspend','DB_events_at_capture','DB_events_after_read','machine_entries_at_capture','tickets_at_capture','pending_at_capture']:
        assert gate[name]==0
    assert gate['context_writes_at_watch']==gate['context_writes_at_capture']==1
    new_phases = phases[:4]+['post_watch_resume_pre_access']+phases[4:]
    expanded = pre['stages']
    assert [s['phase'] for s in expanded]==new_phases
    assert expanded[:4]+expanded[5:]==stages
    assert pre['complete_ordered'] is (captured==11)
    observed = captured+1
    assert [s['sequence'] for s in expanded[:observed]]==list(range(1,observed+1))
    sample = expanded[4]
    assert sample['captured'] and sample['thread']==record['thread'] and sample['origin']=='suspended_GetThreadContext'
    assert all(sample['registers'][reg]==stages[3]['registers'][reg] for reg in ['Dr0','Dr1','Dr2','Dr3'])
    controls = comparison['masks']['Dr7']
    exact = int(controls['MEANINGFUL_EXACT'],16)|int(controls['OS_CONTROLLED_EXACT_OR_REFUSE'],16)
    assert (int(sample['registers']['Dr7'],16)^int(stages[3]['registers']['Dr7'],16))&exact==0
    assert int(sample['registers']['Dr6'],16)&0xe00f==0
    for bit in [11,16]:
        for stage in expanded[:observed]:
            assert stage[f'Dr6_bit{bit}']==(int(stage['registers']['Dr6'],16)>>bit)&1
        changes=[{'from':new_phases[i-1],'to':new_phases[i],'before':expanded[i-1][f'Dr6_bit{bit}'],'after':expanded[i][f'Dr6_bit{bit}']}
                 for i in range(1,observed) if expanded[i-1][f'Dr6_bit{bit}']!=expanded[i][f'Dr6_bit{bit}']]
        result=pre[f'bit{bit}']
        assert result['transitions']==changes and result['changes_observed']==len(changes)
        assert result['first_observed_stage']==(changes[0]['to'] if changes else None)
        expected={'post_foreign_set_readback':'SETTHREADCONTEXT_ROUNDTRIP','post_watch_set_readback_pre_resume':'SETTHREADCONTEXT_ROUNDTRIP',
                  'post_watch_resume_pre_access':'WATCH_ENABLED_RESUME_PRE_ACCESS','outer_db_handler_entry':'DEBUG_EXCEPTION_DELIVERY_BOUNDARY',
                  'outer_db_handler_after_engine':'HANDLER_EDIT','post_return_pre_final_poll':'OS_RETURN_POST_CONTEXT'}
        first=expected.get(changes[0]['to'],'UNRESOLVED_OR_MULTIPLE') if changes else 'UNRESOLVED_OR_MULTIPLE'
        assert result['first_transition_boundary']==(first if captured==11 else 'UNRESOLVED_OR_MULTIPLE')
        if captured<11 or len(changes)>1:
            assert result['classification']=='UNRESOLVED_OR_MULTIPLE' and result['unique'] is False
        elif not changes:
            assert result['classification']=='UNCHANGED' and result['unique'] is True
        else:
            assert result['classification']==first and result['unique'] is (first!='UNRESOLVED_OR_MULTIPLE')
    config = record['retirement_configuration']
    assert record['exact_slot_watch_enabled'] is False and record['RETIREMENT_CONFIGURATION_SAFE'] is False
    assert config['revision']=='c47-v1' and config['RETIREMENT_CONFIGURATION_SAFE'] is False
    assert config['reason']=='native_Windows_required_not_Wine' and config['synthetic_inputs'] is False
    assert config['ticket_capture_complete'] is (scenario!='cap_refusal')
    assert config['ticket_count']==(0 if scenario=='cap_refusal' else record['contexts'])
    for i,ticket in enumerate(config['tickets']):
        assert ticket['sequence']==i+1 and ticket['thread']==record['thread']
        assert ticket['lease']==record['lease'] and ticket['generation']==record['generation']
        assert ticket['acknowledged'] and ticket['current_code']
    assert config['total_context_write_attempts']==(1 if scenario=='cap_refusal' else 2)
    assert config['final_poll_context_write_attempts']==(0 if scenario=='cap_refusal' else 1)
    assert config['unscoped_context_write_attempts']==config['failed_context_write_attempts']==0
    report = record['final_restoration']
    assert report['revision'] == 'c43-v1' and report['comparison'] == 'EXACT_UNMASKED'
    assert report['synthetic_inputs'] is False
    assert set(report['registers']) == {'Dr0', 'Dr1', 'Dr2', 'Dr3', 'Dr6', 'Dr7'}
    if scenario == 'cap_refusal':
        assert record['RAW_EXACT'] is None and record['ARCHITECTURAL_CANONICAL'] is None
        assert record['result'] == 'FAIL_CLOSED_EXPECTED' and record['retirement_accepted'] is False
        assert report['attempted'] is False and report['capture_complete'] is False
        assert report['classification'] == 'NOT_OBSERVED' and report['final_predicate'] is None
        assert report['active_leases_after_final_poll'] is None and report['final_poll_returned'] is None
        assert comparison['readbacks_complete'] is False and comparison['after'] is None
        continue
    assert record['RAW_EXACT'] is comparison['raw_all_exact'] is True
    assert record['ARCHITECTURAL_CANONICAL'] is comparison['architectural_all_equivalent'] is False
    assert record['result'] == 'OWNED_WITNESS_PASS' and record['retirement_accepted'] is True
    assert report['attempted'] is True and report['capture_complete'] is True
    assert report['active_leases_before_final_poll'] is True
    assert report['active_leases_after_final_poll'] is False
    assert report['final_poll_returned'] is False and report['can_retire_before_final_poll'] is True
    assert report['pending_contexts_before_final_poll'] == report['pending_contexts_after_final_poll'] == 0
    assert report['classification'] == 'ALL_RESTORED' and report['final_predicate'] is True
    for name, fields in report['registers'].items():
        assert stages[2]['registers'][name] == fields['saved']
        assert stages[6]['registers'][name] == fields['before']
        assert stages[7]['registers'][name] == fields['after']
    for fields in report['registers'].values():
        saved, before, after = (int(fields[k], 16) for k in ['saved', 'before', 'after'])
        assert fields['before_equal'] == (saved == before)
        assert fields['after_equal'] is True and saved == after
        assert int(fields['after_xor'], 16) == saved ^ after
    assert len(report['verification_readbacks']) == 3
    for check in report['verification_readbacks']:
        assert check['captured'] is True and check['same_saved'] is True
        assert all(value == report['registers'][name]['saved'] for name, value in check['registers'].items())
    assert [s['registers'] for s in stages[8:]] == [c['registers'] for c in report['verification_readbacks']]
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
print('PASS: c47 actual Wine JSON retains12 prior scenarios, gates/raw/c44/c45/c46 evidence, actual ticket/write counts and false native-only configuration predicate')
