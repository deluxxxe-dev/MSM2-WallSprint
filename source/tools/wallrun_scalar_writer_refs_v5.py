"""V5 bounded owner-relative trace with strict no-.pdata fallback.

Reuses V4 authentication/provenance logic. Only the two already-authenticated
downstream roots may use the fallback.
"""
import argparse
from bisect import bisect_right
import hashlib
import json
from pathlib import Path
import struct

import capstone as cs
from capstone import x86 as x

import wallrun_scalar_writer_refs as v4

MAX_NO_PDATA_BYTES = 128
MAX_NO_PDATA_INSTRUCTIONS = 32
NONVOLATILE_OR_RSP = {'rbx','rbp','rsi','rdi','r12','r13','r14','r15','rsp'}


def runtime_containing_or_none(runtimes, starts, target):
    index = bisect_right(starts, target) - 1
    if index < 0:
        return None
    runtime = runtimes[index]
    return runtime if runtime[0] <= target < runtime[1] else None


def executable_section(sections, target):
    found = [
        s for s in sections
        if s['size'] and (s['flags'] & 0x20000000)
        and s['rva'] <= target < s['rva'] + s['size']
    ]
    if len(found) != 1:
        raise v4.Refusal(f'no_pdata_target_not_unique_executable_0x{target:x}')
    return found[0]


def decode_one(md, read, address, window_end):
    size = min(15, window_end - address)
    if size <= 0:
        raise v4.Refusal(f'no_pdata_fallback_window_exhausted_0x{address:x}')
    ins = next(md.disasm(read(address, size), address, count=1), None)
    if ins is None or ins.address != address or ins.size <= 0:
        raise v4.Refusal(f'no_pdata_fallback_decode_failed_0x{address:x}')
    return ins


def has_cycle(edges, start):
    visiting = set()
    visited = set()

    def visit(node):
        if node in visiting:
            return True
        if node in visited:
            return False
        visiting.add(node)
        for nxt in edges.get(node, ()):
            if visit(nxt):
                return True
        visiting.remove(node)
        visited.add(node)
        return False

    return visit(start)


def decode_no_pdata_scope(md, runtimes, starts, read, sections, target):
    sec = executable_section(sections, target)
    section_end = sec['rva'] + sec['size']
    next_index = bisect_right(starts, target)
    next_runtime = starts[next_index] if next_index < len(starts) else section_end
    window_end = min(target + MAX_NO_PDATA_BYTES, section_end, next_runtime)
    if window_end <= target:
        raise v4.Refusal(f'no_pdata_fallback_empty_window_0x{target:x}')

    by_address = {}
    edges = {}
    terminals = {}
    tail_jumps = []
    queue = [target]
    queued = {target}

    while queue:
        address = queue.pop(0)
        queued.discard(address)
        if address in by_address:
            continue
        if not target <= address < window_end:
            raise v4.Refusal(f'no_pdata_fallback_cfg_escape_0x{address:x}')
        if len(by_address) >= MAX_NO_PDATA_INSTRUCTIONS:
            raise v4.Refusal(f'no_pdata_fallback_instruction_cap_0x{target:x}')

        ins = decode_one(md, read, address, window_end)
        by_address[address] = ins

        if ins.group(cs.CS_GRP_RET):
            edges[address] = []
            terminals[address] = 'RET'
            continue
        if ins.group(cs.CS_GRP_CALL):
            raise v4.Refusal(f'no_pdata_fallback_call_0x{ins.address:x}')

        if v4.written_gprs(ins) & NONVOLATILE_OR_RSP:
            raise v4.Refusal(f'no_pdata_fallback_nonvolatile_or_rsp_write_0x{ins.address:x}')

        fallthrough = ins.address + ins.size
        successors = []
        if ins.group(cs.CS_GRP_JUMP):
            if not ins.operands or ins.operands[0].type != x.X86_OP_IMM:
                raise v4.Refusal(f'no_pdata_fallback_indirect_jump_0x{ins.address:x}')
            jump_target = int(ins.operands[0].imm)
            if ins.id == x.X86_INS_JMP:
                if target <= jump_target < window_end:
                    successors.append(jump_target)
                else:
                    terminals[address] = 'DIRECT_EXTERNAL_TAIL_JUMP'
                    tail_jumps.append({
                        'instruction_rva': ins.address,
                        'target_rva': jump_target,
                        'instruction_hex': ins.bytes.hex(),
                    })
            else:
                if not target <= jump_target < window_end:
                    raise v4.Refusal(f'no_pdata_fallback_conditional_escape_0x{ins.address:x}')
                if not target <= fallthrough < window_end:
                    raise v4.Refusal(f'no_pdata_fallback_fallthrough_escape_0x{ins.address:x}')
                successors.extend((jump_target, fallthrough))
        else:
            if not target <= fallthrough < window_end:
                raise v4.Refusal(f'no_pdata_fallback_no_terminator_before_window_0x{target:x}')
            successors.append(fallthrough)

        edges[address] = list(dict.fromkeys(successors))
        for nxt in edges[address]:
            if nxt not in by_address and nxt not in queued:
                queue.append(nxt)
                queued.add(nxt)

    if not terminals:
        raise v4.Refusal(f'no_pdata_fallback_no_terminal_0x{target:x}')
    for successors in edges.values():
        for nxt in successors:
            if nxt not in by_address:
                raise v4.Refusal(f'no_pdata_fallback_unresolved_successor_0x{nxt:x}')
    if has_cycle(edges, target):
        raise v4.Refusal(f'no_pdata_fallback_cfg_cycle_0x{target:x}')
    for address in by_address:
        if not edges.get(address) and address not in terminals:
            raise v4.Refusal(f'no_pdata_fallback_unproven_exit_0x{address:x}')

    instructions = [by_address[a] for a in sorted(by_address)]
    digest_material = b''.join(
        struct.pack('<Q', ins.address) + bytes(ins.bytes) for ins in instructions
    )
    return {
        'begin_rva': target,
        'end_rva': window_end,
        'unwind_rva': None,
        'size': sum(ins.size for ins in instructions),
        'sha256': hashlib.sha256(digest_material).hexdigest(),
        'instructions': instructions,
        'scope_kind': 'STRICT_NO_PDATA_BOUNDED_CFG',
        'runtime_backed': False,
        'window_end_rva': window_end,
        'external_tail_jumps': tail_jumps,
        'terminal_count': len(terminals),
    }


def decode_target_scope(md, runtimes, starts, read, sections, target):
    runtime = runtime_containing_or_none(runtimes, starts, target)
    if runtime is not None:
        info = v4.decode_runtime_function(md, runtime, read)
        if target not in {ins.address for ins in info['instructions']}:
            raise v4.Refusal(f'downstream_target_not_instruction_boundary_0x{target:x}')
        info['scope_kind'] = 'PE_RUNTIME_FUNCTION'
        info['runtime_backed'] = True
        info['external_tail_jumps'] = []
        return info
    return decode_no_pdata_scope(md, runtimes, starts, read, sections, target)


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

    traces = []
    accesses = []
    for root in v4.ROOTS:
        info = decode_target_scope(
            md, runtimes, starts, read, sections, root['target_rva']
        )
        trace = v4.trace_root(
            info, root['target_rva'], root['entry_owner_offset']
        )
        accesses.extend(trace['owner_relative_0x250_accesses'])
        traces.append({
            'name': root['name'],
            'target_rva': root['target_rva'],
            'scope_kind': info['scope_kind'],
            'runtime_backed': info['runtime_backed'],
            'scope_begin_rva': info['begin_rva'],
            'scope_end_rva': info['end_rva'],
            'unwind_rva': info['unwind_rva'],
            'scope_sha256': info['sha256'],
            'entry_owner_offset': root['entry_owner_offset'],
            'target_relative_field_offset': root['target_relative_field_offset'],
            'external_tail_jumps': info.get('external_tail_jumps', []),
            'trace': trace,
        })

    writes = [row for row in accesses if row['write']]
    reads = [row for row in accesses if row['read']]
    unknown = [row for row in accesses if not row['access_known']]
    if writes:
        result_name = 'OWNER_RELATIVE_0X250_WRITER_FOUND_IN_BOUNDED_DOWNSTREAM_TRACE'
    elif accesses:
        result_name = 'OWNER_RELATIVE_0X250_ACCESS_FOUND_NO_WRITE_IN_BOUNDED_DOWNSTREAM_TRACE'
    else:
        result_name = 'NO_OWNER_RELATIVE_0X250_ACCESS_IN_BOUNDED_DOWNSTREAM_TRACE'

    result = {
        'schema': 5,
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
        },
        'downstream_traces': traces,
        'summary': {
            'owner_relative_0x250_access_count': len(accesses),
            'read_count': len(reads),
            'write_count': len(writes),
            'access_unknown_count': len(unknown),
        },
        'scope': {
            'roots_only': ['0x29c06f0', '0x29c18b0'],
            'global_executable_scan': False,
            'adjacent_offsets_scanned': False,
            'no_pdata_fallback': 'STRICT_MAX_128_BYTES_32_REACHABLE_INSNS_NO_CALL_NO_NONVOLATILE_OR_RSP_WRITE_NO_INDIRECT_JUMP_NO_CFG_CYCLE',
            'memory_load_provenance': False,
            'indexed_computed_addresses': False,
            'deeper_callee_traversal': False,
            'runtime_hooks_or_watchpoints': False,
            'live_reads_or_writes': 0,
            'game_process_operation': False,
            'gameplay_write_authorized': False,
            'field_semantics': 'UNKNOWN',
        },
        'interpretation': 'bounded static owner-relative GPR provenance only; no-.pdata targets use strict bounded CFG fallback; matching access proves address relation inside this model, not field semantics or safe writability',
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
            'schema': 5,
            'result': 'REFUSED',
            'reason': str(exc),
            'live_reads_or_writes': 0,
            'game_process_operation': False,
        }
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
        print(f'REFUSED: {exc}')
        return 2

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(
        f"{result['result']}: "
        f"accesses={result['summary']['owner_relative_0x250_access_count']} "
        f"writes={result['summary']['write_count']} output={args.output}"
    )
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
