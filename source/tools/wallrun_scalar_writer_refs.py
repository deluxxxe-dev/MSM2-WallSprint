"""Pinned offline owner-relative provenance trace for WallRun.this+0x250.

The trace is restricted to two downstream roots already authenticated from WallRun
Update through 0x367DD10. It never launches/attaches to the game and never writes
memory. Provenance is conservative: only a small set of full-64-bit GPR transforms
is modeled, CFG merges use exact intersection, and volatile Win64 GPRs are killed
after every call.
"""
import argparse
from bisect import bisect_right
import hashlib
import json
from pathlib import Path
import struct

import capstone as cs
from capstone import x86 as x
import discover_ground_support as peutil

EXPECTED_SHA = peutil.EXPECTED_SHA
UPDATE_BEGIN = 0x1424530
UPDATE_END = 0x14246D6
UPDATE_SHA256 = 'e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f'
SOURCE_BEGIN = 0x367DD10
SOURCE_END = 0x367DD5F
SOURCE_SHA256 = 'f437f092ce0b2315103302d06cdefb5ded88126302b3d276bb1a6ebcbf774378'
FIELD_OFFSET = 0x250
KNOWN_UPDATE_READS = (0x1424585, 0x142460E)
KNOWN_UPDATE_OWNER_CALL = (0x14245C3, SOURCE_BEGIN)
KNOWN_UPDATE_CALL_PREFIX = bytes.fromhex('48 8b ce e8 48 97 25 02')

ROOTS = (
    {
        'name': 'A_owner_plus_0x80',
        'adjust_rva': 0x367DD2D,
        'adjust_bytes': bytes.fromhex('48 83 e9 80'),
        'call_rva': 0x367DD31,
        'call_bytes': bytes.fromhex('e8 ba 29 34 ff'),
        'target_rva': 0x29C06F0,
        'entry_owner_offset': 0x80,
        'target_relative_field_offset': 0x1D0,
    },
    {
        'name': 'B_owner_plus_0x58',
        'adjust_rva': 0x367DD38,
        'adjust_bytes': bytes.fromhex('48 83 c1 58'),
        'call_rva': 0x367DD3C,
        'call_bytes': bytes.fromhex('e8 6f 3b 34 ff'),
        'target_rva': 0x29C18B0,
        'entry_owner_offset': 0x58,
        'target_relative_field_offset': 0x1F8,
    },
)

MAX_FILE = peutil.MAX_FILE
MAX_RUNTIME_TABLE = 8 * 1024 * 1024
MAX_RUNTIME_ENTRIES = 600_000
MAX_FUNCTION_BYTES = 256 * 1024
MAX_FUNCTION_INSTRUCTIONS = 65_536
MAX_TRACE_VISITS = 200_000
MAX_REACHABLE_CALLS = 2_048
MAX_MATCHED_ACCESSES = 256
MAX_OUTPUT = 512 * 1024

REG64 = {
    'rax','rbx','rcx','rdx','rsi','rdi','rbp','rsp',
    'r8','r9','r10','r11','r12','r13','r14','r15',
}
VOLATILE_WIN64 = {'rax','rcx','rdx','r8','r9','r10','r11'}
REG_PARENT = {
    'rax': 'rax', 'eax': 'rax', 'ax': 'rax', 'al': 'rax', 'ah': 'rax',
    'rbx': 'rbx', 'ebx': 'rbx', 'bx': 'rbx', 'bl': 'rbx', 'bh': 'rbx',
    'rcx': 'rcx', 'ecx': 'rcx', 'cx': 'rcx', 'cl': 'rcx', 'ch': 'rcx',
    'rdx': 'rdx', 'edx': 'rdx', 'dx': 'rdx', 'dl': 'rdx', 'dh': 'rdx',
    'rsi': 'rsi', 'esi': 'rsi', 'si': 'rsi', 'sil': 'rsi',
    'rdi': 'rdi', 'edi': 'rdi', 'di': 'rdi', 'dil': 'rdi',
    'rbp': 'rbp', 'ebp': 'rbp', 'bp': 'rbp', 'bpl': 'rbp',
    'rsp': 'rsp', 'esp': 'rsp', 'sp': 'rsp', 'spl': 'rsp',
}
for _i in range(8,16):
    REG_PARENT[f'r{_i}'] = f'r{_i}'
    REG_PARENT[f'r{_i}d'] = f'r{_i}'
    REG_PARENT[f'r{_i}w'] = f'r{_i}'
    REG_PARENT[f'r{_i}b'] = f'r{_i}'


class Refusal(ValueError):
    pass


def decoder():
    if cs.__version__ != '5.0.6':
        raise Refusal('requires_Capstone_5.0.6')
    md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_64)
    md.detail = True
    return md


def parse_runtime_table(data, sections, image_size):
    def section(at, size):
        found = [s for s in sections if s['size'] and s['rva'] <= at and at + size <= s['rva'] + s['size']]
        if len(found) != 1:
            raise Refusal('range_not_uniquely_file_backed')
        return found[0]

    def read(at, size):
        s = section(at, size)
        off = s['raw'] + at - s['rva']
        return data[off:off + size]

    pe = struct.unpack_from('<I', data, 60)[0]
    op = pe + 24
    if struct.unpack_from('<I', data, op + 108)[0] < 4:
        raise Refusal('exception_directory_not_declared')
    table, size = struct.unpack_from('<II', data, op + 112 + 3 * 8)
    if not size or size % 12 or size > MAX_RUNTIME_TABLE or size // 12 > MAX_RUNTIME_ENTRIES:
        raise Refusal('runtime_table_missing_or_cap')
    table_section = section(table, size)
    if not (table_section['flags'] & 0x40000000) or table_section['flags'] & 0x80000000:
        raise Refusal('runtime_table_not_readonly')

    rows = []
    previous_end = 0
    for begin, end, unwind in struct.iter_unpack('<III', read(table, size)):
        if not 0 < begin < end <= image_size or begin < previous_end or unwind % 4:
            raise Refusal('invalid_runtime_table_order_or_range')
        sec = section(begin, end - begin)
        if not sec['flags'] & 0x20000000:
            raise Refusal('runtime_function_not_executable')
        hdr = read(unwind, 4)
        version, flags = hdr[0] & 7, hdr[0] >> 3
        if version not in (1, 2) or flags & ~7 or (flags & 4 and flags & 3):
            raise Refusal('invalid_unwind_header')
        unwind_size = 4 + ((hdr[2] + 1) // 2) * 4 + (12 if flags & 4 else 4 if flags & 3 else 0)
        read(unwind, unwind_size)
        rows.append((begin, end, unwind))
        previous_end = end
    return rows, read


def decode_runtime_function(md, runtime, read):
    begin, end, unwind = runtime
    size = end - begin
    if not 0 < size <= MAX_FUNCTION_BYTES:
        raise Refusal(f'target_function_size_cap_0x{begin:x}')
    body = read(begin, size)
    instructions = list(md.disasm(body, begin))
    if not instructions or len(instructions) > MAX_FUNCTION_INSTRUCTIONS:
        raise Refusal(f'target_instruction_cap_0x{begin:x}')
    cursor = begin
    for ins in instructions:
        if ins.address != cursor:
            raise Refusal(f'target_decode_gap_0x{begin:x}')
        cursor += ins.size
    if cursor != end:
        raise Refusal(f'target_decode_incomplete_0x{begin:x}')
    return {
        'begin_rva': begin,
        'end_rva': end,
        'unwind_rva': unwind,
        'size': size,
        'sha256': hashlib.sha256(body).hexdigest(),
        'instructions': instructions,
    }


def runtime_containing(runtimes, starts, target):
    index = bisect_right(starts, target) - 1
    if index < 0:
        raise Refusal(f'downstream_target_no_runtime_function_0x{target:x}')
    runtime = runtimes[index]
    if not (runtime[0] <= target < runtime[1]):
        raise Refusal(f'downstream_target_no_runtime_function_0x{target:x}')
    return runtime


def parent_reg(ins, reg_id):
    if not reg_id:
        return None
    return REG_PARENT.get(ins.reg_name(reg_id))


def full64_reg(ins, operand):
    if operand.type != x.X86_OP_REG or operand.size != 8:
        return None
    name = ins.reg_name(operand.reg)
    return name if name in REG64 else None


def merge_intersection(a, b):
    return {reg: value for reg, value in a.items() if reg in b and b[reg] == value}


def written_gprs(ins):
    try:
        _, writes = ins.regs_access()
    except cs.CsError:
        writes = []
    result = set()
    for reg_id in writes:
        reg = parent_reg(ins, reg_id)
        if reg:
            result.add(reg)
    return result


def transfer(ins, state):
    before = dict(state)
    after = dict(state)
    for reg in written_gprs(ins):
        after.pop(reg, None)

    ops = ins.operands
    if ins.id == x.X86_INS_MOV and len(ops) >= 2:
        dst = full64_reg(ins, ops[0])
        src = full64_reg(ins, ops[1])
        if dst and src and src in before:
            after[dst] = before[src]
    elif ins.id == x.X86_INS_LEA and len(ops) >= 2:
        dst = full64_reg(ins, ops[0])
        mem = ops[1]
        if dst and mem.type == x.X86_OP_MEM and not mem.mem.index:
            base = parent_reg(ins, mem.mem.base)
            if base and base in before:
                after[dst] = before[base] + int(mem.mem.disp)
    elif ins.id in (x.X86_INS_ADD, x.X86_INS_SUB) and len(ops) >= 2:
        dst = full64_reg(ins, ops[0])
        imm = ops[1]
        if dst and imm.type == x.X86_OP_IMM and dst in before:
            delta = int(imm.imm)
            after[dst] = before[dst] + (delta if ins.id == x.X86_INS_ADD else -delta)

    if ins.group(cs.CS_GRP_CALL):
        for reg in VOLATILE_WIN64:
            after.pop(reg, None)
    return after


def successors(ins, by_address, function_begin, function_end):
    fallthrough = ins.address + ins.size
    if ins.group(cs.CS_GRP_RET):
        return []
    if ins.group(cs.CS_GRP_JUMP):
        if not ins.operands or ins.operands[0].type != x.X86_OP_IMM:
            raise Refusal(f'entry_reachable_indirect_jump_0x{ins.address:x}')
        target = int(ins.operands[0].imm)
        rows = []
        if function_begin <= target < function_end:
            if target not in by_address:
                raise Refusal(f'cfg_target_not_instruction_boundary_0x{target:x}')
            rows.append(target)
        if ins.id != x.X86_INS_JMP and function_begin <= fallthrough < function_end:
            if fallthrough not in by_address:
                raise Refusal(f'cfg_fallthrough_not_instruction_boundary_0x{fallthrough:x}')
            rows.append(fallthrough)
        return rows
    if function_begin <= fallthrough < function_end:
        if fallthrough not in by_address:
            raise Refusal(f'cfg_fallthrough_not_instruction_boundary_0x{fallthrough:x}')
        return [fallthrough]
    return []


def trace_root(info, target_rva, entry_owner_offset):
    instructions = info['instructions']
    by_address = {ins.address: ins for ins in instructions}
    if target_rva not in by_address:
        raise Refusal(f'downstream_target_not_instruction_boundary_0x{target_rva:x}')

    in_state = {target_rva: {'rcx': entry_owner_offset}}
    queue = [target_rva]
    queued = {target_rva}
    visits = 0
    while queue:
        address = queue.pop(0)
        queued.discard(address)
        visits += 1
        if visits > MAX_TRACE_VISITS:
            raise Refusal(f'owner_provenance_trace_visit_cap_0x{target_rva:x}')
        ins = by_address[address]
        out_state = transfer(ins, in_state[address])
        for nxt in successors(ins, by_address, info['begin_rva'], info['end_rva']):
            if nxt not in in_state:
                in_state[nxt] = dict(out_state)
                changed = True
            else:
                merged = merge_intersection(in_state[nxt], out_state)
                changed = merged != in_state[nxt]
                if changed:
                    in_state[nxt] = merged
            if changed and nxt not in queued:
                queue.append(nxt)
                queued.add(nxt)

    accesses = []
    calls = []
    reachable = sorted(in_state)
    for address in reachable:
        ins = by_address[address]
        state = in_state[address]
        for op in ins.operands:
            if op.type != x.X86_OP_MEM or op.mem.index:
                continue
            base = parent_reg(ins, op.mem.base)
            if not base or base not in state:
                continue
            effective = state[base] + int(op.mem.disp)
            if effective != FIELD_OFFSET:
                continue
            if len(accesses) >= MAX_MATCHED_ACCESSES:
                raise Refusal(f'owner_relative_access_cap_0x{target_rva:x}')
            accesses.append({
                'instruction_rva': ins.address,
                'mnemonic': ins.mnemonic,
                'operands': ins.op_str,
                'instruction_hex': ins.bytes.hex(),
                'base_register': base,
                'base_owner_offset': state[base],
                'encoded_displacement': int(op.mem.disp),
                'effective_owner_offset': effective,
                'width': op.size,
                'read': bool(op.access & cs.CS_AC_READ),
                'write': bool(op.access & cs.CS_AC_WRITE),
                'access_known': bool(op.access),
                'owner_provenance': 'PROVEN_STATIC_WITHIN_BOUNDED_CFG_MODEL',
                'field_semantics_proven': False,
            })
        if ins.group(cs.CS_GRP_CALL) and ins.operands and ins.operands[0].type == x.X86_OP_IMM:
            if len(calls) >= MAX_REACHABLE_CALLS:
                raise Refusal(f'owner_relative_call_cap_0x{target_rva:x}')
            rcx = state.get('rcx')
            calls.append({
                'call_site_rva': ins.address,
                'target_rva': int(ins.operands[0].imm),
                'proven_rcx_owner_offsets': [] if rcx is None else [rcx],
                'pre_call_state': dict(sorted(state.items())),
                'call_return_volatile_gprs_invalidated': sorted(VOLATILE_WIN64),
            })

    return {
        'reachable_instruction_count': len(reachable),
        'worklist_visits': visits,
        'entry_state': {'rcx': entry_owner_offset},
        'owner_relative_0x250_accesses': accesses,
        'reachable_direct_calls': calls,
        'trace_complete_for_supported_cfg': True,
        'provenance_model': {
            'supported_transforms': ['MOV_reg64_reg64','LEA_reg64_known_base_plus_constant_no_index','ADD_reg64_imm','SUB_reg64_imm'],
            'unsupported_or_partial_writes': 'KILL_DESTINATION_PROVENANCE',
            'memory_load_destination': 'KILL_DESTINATION_PROVENANCE',
            'cfg_merge': 'INTERSECTION_SAME_REGISTER_SAME_OWNER_OFFSET_ONLY',
            'call_clobber': 'WIN64_VOLATILE_GPRS_RAX_RCX_RDX_R8_R9_R10_R11',
            'indexed_address': 'NOT_PROVEN',
            'deeper_callee_traversal': False,
        },
    }


def validate_source_relation(md, runtime_by_begin, read):
    update = runtime_by_begin.get(UPDATE_BEGIN)
    source = runtime_by_begin.get(SOURCE_BEGIN)
    if update is None or source is None:
        raise Refusal('authenticated_source_runtime_entry_missing')
    update_info = decode_runtime_function(md, update, read)
    source_info = decode_runtime_function(md, source, read)
    if update_info['end_rva'] != UPDATE_END or update_info['sha256'] != UPDATE_SHA256:
        raise Refusal('authenticated_Update_identity_mismatch')
    if source_info['end_rva'] != SOURCE_END or source_info['sha256'] != SOURCE_SHA256:
        raise Refusal('authenticated_source_identity_mismatch')
    if read(KNOWN_UPDATE_OWNER_CALL[0] - 3, len(KNOWN_UPDATE_CALL_PREFIX)) != KNOWN_UPDATE_CALL_PREFIX:
        raise Refusal('authenticated_Update_owner_call_prefix_mismatch')
    update_by = {ins.address: ins for ins in update_info['instructions']}
    call = update_by.get(KNOWN_UPDATE_OWNER_CALL[0])
    if call is None or call.id != x.X86_INS_CALL or not call.operands or call.operands[0].type != x.X86_OP_IMM or int(call.operands[0].imm) != SOURCE_BEGIN:
        raise Refusal('authenticated_Update_owner_call_target_mismatch')
    update_sites = []
    for ins in update_info['instructions']:
        for op in ins.operands:
            if op.type == x.X86_OP_MEM and op.mem.disp == FIELD_OFFSET:
                update_sites.append(ins.address)
                break
    if tuple(sorted(update_sites)) != KNOWN_UPDATE_READS:
        raise Refusal('known_Update_disp250_site_selfcheck_failed')

    source_by = {ins.address: ins for ins in source_info['instructions']}
    root_rows = []
    for root in ROOTS:
        if root['target_relative_field_offset'] != FIELD_OFFSET - root['entry_owner_offset']:
            raise Refusal(f'root_effective_offset_invariant_mismatch_{root["name"]}')
        if read(root['adjust_rva'], len(root['adjust_bytes'])) != root['adjust_bytes']:
            raise Refusal(f'root_adjust_bytes_mismatch_{root["name"]}')
        if read(root['call_rva'], len(root['call_bytes'])) != root['call_bytes']:
            raise Refusal(f'root_call_bytes_mismatch_{root["name"]}')
        call_ins = source_by.get(root['call_rva'])
        if call_ins is None or call_ins.id != x.X86_INS_CALL or not call_ins.operands or call_ins.operands[0].type != x.X86_OP_IMM or int(call_ins.operands[0].imm) != root['target_rva']:
            raise Refusal(f'root_call_target_mismatch_{root["name"]}')
        root_rows.append({
            'name': root['name'],
            'adjust_rva': root['adjust_rva'],
            'adjust_hex': root['adjust_bytes'].hex(),
            'call_rva': root['call_rva'],
            'call_hex': root['call_bytes'].hex(),
            'target_rva': root['target_rva'],
            'entry_owner_offset': root['entry_owner_offset'],
            'target_relative_field_offset': root['target_relative_field_offset'],
        })
    return update_info, source_info, root_rows


def scan(data):
    if len(data) > MAX_FILE:
        raise Refusal('file_cap')
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED_SHA:
        raise Refusal('unsupported_build_sha256')

    _, image_size, sections = peutil.pe_sections(data)
    runtimes, read = parse_runtime_table(data, sections, image_size)
    starts = [row[0] for row in runtimes]
    runtime_by_begin = {row[0]: row for row in runtimes}
    md = decoder()

    update_info, source_info, source_roots = validate_source_relation(md, runtime_by_begin, read)

    trace_rows = []
    all_accesses = []
    for root in ROOTS:
        runtime = runtime_containing(runtimes, starts, root['target_rva'])
        info = decode_runtime_function(md, runtime, read)
        trace = trace_root(info, root['target_rva'], root['entry_owner_offset'])
        all_accesses.extend(trace['owner_relative_0x250_accesses'])
        trace_rows.append({
            'name': root['name'],
            'target_rva': root['target_rva'],
            'target_is_exact_runtime_entry': runtime[0] == root['target_rva'],
            'containing_runtime_function': {
                'begin_rva': info['begin_rva'],
                'end_rva': info['end_rva'],
                'unwind_rva': info['unwind_rva'],
                'size': info['size'],
                'sha256': info['sha256'],
            },
            'entry_owner_offset': root['entry_owner_offset'],
            'target_relative_field_offset': root['target_relative_field_offset'],
            'trace': trace,
        })

    writes = [row for row in all_accesses if row['write']]
    reads = [row for row in all_accesses if row['read']]
    unknown = [row for row in all_accesses if not row['access_known']]
    if writes:
        result_name = 'OWNER_RELATIVE_0X250_WRITER_FOUND_IN_BOUNDED_DOWNSTREAM_TRACE'
    elif all_accesses:
        result_name = 'OWNER_RELATIVE_0X250_ACCESS_FOUND_NO_WRITE_IN_BOUNDED_DOWNSTREAM_TRACE'
    else:
        result_name = 'NO_OWNER_RELATIVE_0X250_ACCESS_IN_BOUNDED_DOWNSTREAM_TRACE'

    result = {
        'schema': 4,
        'result': result_name,
        'exe_sha256': digest,
        'decoder': cs.__version__,
        'field_owner_offset': FIELD_OFFSET,
        'authenticated_source': {
            'update': {
                'begin_rva': update_info['begin_rva'],
                'end_rva': update_info['end_rva'],
                'sha256': update_info['sha256'],
                'known_disp250_sites': list(KNOWN_UPDATE_READS),
            },
            'owner_callee': {
                'begin_rva': source_info['begin_rva'],
                'end_rva': source_info['end_rva'],
                'sha256': source_info['sha256'],
            },
            'pinned_roots': source_roots,
        },
        'downstream_traces': trace_rows,
        'summary': {
            'owner_relative_0x250_access_count': len(all_accesses),
            'read_count': len(reads),
            'write_count': len(writes),
            'access_unknown_count': len(unknown),
        },
        'scope': {
            'roots_only': ['0x29c06f0','0x29c18b0'],
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
        'interpretation': 'bounded static owner-relative GPR provenance only; matching access proves address relation inside the supported CFG model, not field semantics or safe writability',
    }
    payload = (json.dumps(result, indent=2) + '\n').encode()
    if len(payload) > MAX_OUTPUT:
        raise Refusal('output_cap')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    try:
        data = args.exe.read_bytes()
        result = scan(data)
    except (OSError, Refusal, peutil.Refusal) as exc:
        result = {
            'schema': 4,
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
    print(f"{result['result']}: accesses={result['summary']['owner_relative_0x250_access_count']} writes={result['summary']['write_count']} output={args.output}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
