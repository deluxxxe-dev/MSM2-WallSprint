"""V7 one-step owner-relative provenance trace for the single V6 Path-B handoff.

This helper re-authenticates the accepted WallRun Update/source roots, reproduces
the V6 Path-B trace, pins the single reachable direct handoff
0x29C18D4 -> 0x308A0F0 with RCX = owner+0x58, then traces only that callee.
No whole-image scan, adjacent-offset scan, live process operation, or game write
is performed.
"""
import argparse
import hashlib
import json
from pathlib import Path

import capstone as cs
from capstone import x86 as x

import wallrun_scalar_writer_refs as v4
import wallrun_scalar_writer_refs_v6 as v6

PATH_B_TARGET = 0x29C18B0
PATH_B_ENTRY_OWNER_OFFSET = 0x58
HANDOFF_CALL_SITE = 0x29C18D4
HANDOFF_CALL_BYTES = bytes.fromhex('e817886c00')
HANDOFF_TARGET = 0x308A0F0
HANDOFF_ENTRY_OWNER_OFFSET = 0x58


def authenticate_handoff(md, runtimes, starts, read, sections):
    info = v6.decode_target_scope(
        md, runtimes, starts, read, sections, PATH_B_TARGET
    )
    trace = v4.trace_root(info, PATH_B_TARGET, PATH_B_ENTRY_OWNER_OFFSET)

    if read(HANDOFF_CALL_SITE, len(HANDOFF_CALL_BYTES)) != HANDOFF_CALL_BYTES:
        raise v4.Refusal('v6_handoff_call_bytes_mismatch')

    by_address = {ins.address: ins for ins in info['instructions']}
    call_ins = by_address.get(HANDOFF_CALL_SITE)
    if (
        call_ins is None
        or call_ins.id != x.X86_INS_CALL
        or not call_ins.operands
        or call_ins.operands[0].type != x.X86_OP_IMM
        or int(call_ins.operands[0].imm) != HANDOFF_TARGET
    ):
        raise v4.Refusal('v6_handoff_call_target_mismatch')

    rows = [
        row for row in trace['reachable_direct_calls']
        if row['call_site_rva'] == HANDOFF_CALL_SITE
    ]
    if len(rows) != 1:
        raise v4.Refusal('v6_handoff_call_reachability_mismatch')
    row = rows[0]
    if row['target_rva'] != HANDOFF_TARGET:
        raise v4.Refusal('v6_handoff_target_report_mismatch')
    if row['proven_rcx_owner_offsets'] != [HANDOFF_ENTRY_OWNER_OFFSET]:
        raise v4.Refusal('v6_handoff_rcx_owner_offset_mismatch')

    return {
        'source_path': 'B_owner_plus_0x58',
        'source_target_rva': PATH_B_TARGET,
        'call_site_rva': HANDOFF_CALL_SITE,
        'call_hex': HANDOFF_CALL_BYTES.hex(),
        'target_rva': HANDOFF_TARGET,
        'entry_rcx_owner_offset': HANDOFF_ENTRY_OWNER_OFFSET,
        'pre_call_state': row['pre_call_state'],
        'source_scope_kind': info['scope_kind'],
        'source_scope_sha256': info['sha256'],
    }


def scan(data):
    if len(data) > v4.MAX_FILE:
        raise v4.Refusal('file_cap')
    digest = hashlib.sha256(data).hexdigest()
    if digest != v4.EXPECTED_SHA:
        raise v4.Refusal('unsupported_build_sha256')

    _, image_size, sections = v4.peutil.pe_sections(data)
    runtimes, read = v4.parse_runtime_table(data, sections, image_size)
    starts = [row[0] for row in runtimes]
    runtime_by_begin = {row[0]: row for row in runtimes}
    md = v4.decoder()

    update_info, source_info, source_roots = v4.validate_source_relation(
        md, runtime_by_begin, read
    )
    handoff = authenticate_handoff(md, runtimes, starts, read, sections)

    info = v6.decode_target_scope(
        md, runtimes, starts, read, sections, HANDOFF_TARGET
    )
    trace = v4.trace_root(
        info, HANDOFF_TARGET, HANDOFF_ENTRY_OWNER_OFFSET
    )
    if (
        not info['runtime_backed']
        and trace['worklist_visits'] > v6.MAX_NO_PDATA_TRACE_VISITS
    ):
        raise v4.Refusal(
            f'no_pdata_provenance_worklist_cap_0x{HANDOFF_TARGET:x}'
        )

    accesses = trace['owner_relative_0x250_accesses']
    writes = [row for row in accesses if row['write']]
    reads = [row for row in accesses if row['read']]
    unknown = [row for row in accesses if not row['access_known']]

    if writes:
        result_name = 'OWNER_RELATIVE_0X250_WRITER_FOUND_IN_V7_HANDOFF_CALLEE'
    elif accesses:
        result_name = 'OWNER_RELATIVE_0X250_ACCESS_FOUND_NO_WRITE_IN_V7_HANDOFF_CALLEE'
    else:
        result_name = 'NO_OWNER_RELATIVE_0X250_ACCESS_IN_V7_HANDOFF_CALLEE'

    result = {
        'schema': 7,
        'result': result_name,
        'exe_sha256': digest,
        'decoder': cs.__version__,
        'field_owner_offset': v4.FIELD_OFFSET,
        'authenticated_source': {
            'update': {
                'begin_rva': update_info['begin_rva'],
                'end_rva': update_info['end_rva'],
                'sha256': update_info['sha256'],
                'known_disp250_sites': list(v4.KNOWN_UPDATE_READS),
            },
            'owner_callee': {
                'begin_rva': source_info['begin_rva'],
                'end_rva': source_info['end_rva'],
                'sha256': source_info['sha256'],
            },
            'pinned_roots': source_roots,
            'v6_handoff': handoff,
        },
        'handoff_trace': {
            'name': 'C_from_B_owner_plus_0x58',
            'target_rva': HANDOFF_TARGET,
            'scope_kind': info['scope_kind'],
            'runtime_backed': info['runtime_backed'],
            'scope_begin_rva': info['begin_rva'],
            'scope_end_rva': info['end_rva'],
            'unwind_rva': info['unwind_rva'],
            'scope_sha256': info['sha256'],
            'entry_owner_offset': HANDOFF_ENTRY_OWNER_OFFSET,
            'target_relative_field_offset':
                v4.FIELD_OFFSET - HANDOFF_ENTRY_OWNER_OFFSET,
            'external_tail_jumps': info.get('external_tail_jumps', []),
            'cfg_cycle_present': info.get('cfg_cycle_present'),
            'trace': trace,
        },
        'summary': {
            'owner_relative_0x250_access_count': len(accesses),
            'read_count': len(reads),
            'write_count': len(writes),
            'access_unknown_count': len(unknown),
        },
        'scope': {
            'source_path': 'B_only',
            'source_call_site': '0x29c18d4',
            'single_handoff_target': '0x308a0f0',
            'entry_rcx_owner_offset': '0x58',
            'path_a_reopened': False,
            'global_executable_scan': False,
            'adjacent_offsets_scanned': False,
            'memory_load_provenance': False,
            'indexed_computed_addresses': False,
            'deeper_callee_traversal': False,
            'runtime_hooks_or_watchpoints': False,
            'live_reads_or_writes': 0,
            'game_process_operation': False,
            'gameplay_write_authorized': False,
            'field_semantics': 'UNKNOWN',
        },
        'interpretation': (
            'one-step bounded static owner-relative GPR provenance from the '
            'single V6 Path-B call that preserved RCX=owner+0x58; matching '
            'access proves only address relation inside the supported model, '
            'not field semantics or safe writability'
        ),
    }
    payload = (json.dumps(result, indent=2) + '\n').encode()
    if len(payload) > v4.MAX_OUTPUT:
        raise v4.Refusal('output_cap')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    try:
        result = scan(args.exe.read_bytes())
    except (OSError, v4.Refusal, v4.peutil.Refusal) as exc:
        result = {
            'schema': 7,
            'result': 'REFUSED',
            'reason': str(exc),
            'live_reads_or_writes': 0,
            'game_process_operation': False,
        }
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(
            json.dumps(result, indent=2) + '\n', encoding='utf-8'
        )
        print(f'REFUSED: {exc}')
        return 2

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(
        json.dumps(result, indent=2) + '\n', encoding='utf-8'
    )
    print(
        f"{result['result']}: "
        f"accesses={result['summary']['owner_relative_0x250_access_count']} "
        f"writes={result['summary']['write_count']} output={args.output}"
    )
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
