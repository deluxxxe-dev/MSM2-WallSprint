"""V8 bounded consumer trace for authenticated WallRun.this+0x250 reads.

This tool does not search for writers. It authenticates the exact WallRun Update
body and emits small bounded CFG slices from the two already-known +0x250 read
sites, including register access and file-backed RIP-relative operands. It never
launches or attaches to the game and never writes game memory.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

import capstone as cs
from capstone import x86 as x

import wallrun_scalar_writer_refs as v4

SCHEMA = 8
READ_SITES = v4.KNOWN_UPDATE_READS
MAX_PRE_CONTEXT = 6
MAX_REACHABLE_INSTRUCTIONS = 32
MAX_WORKLIST_VISITS = 96
MAX_OUTPUT = 512 * 1024


def reg_names(ins, ids):
    return [ins.reg_name(reg) for reg in ids]


def memory_operand(ins, op, read):
    base = ins.reg_name(op.mem.base) if op.mem.base else None
    index = ins.reg_name(op.mem.index) if op.mem.index else None
    row = {
        'base': base,
        'index': index,
        'scale': int(op.mem.scale),
        'displacement': int(op.mem.disp),
        'width': int(op.size),
        'read': bool(op.access & cs.CS_AC_READ),
        'write': bool(op.access & cs.CS_AC_WRITE),
        'access_known': bool(op.access),
    }
    if base == 'rip':
        target = ins.address + ins.size + int(op.mem.disp)
        row['rip_target_rva'] = target
        if 0 < op.size <= 16:
            try:
                raw = read(target, int(op.size))
            except Exception:
                raw = None
            if raw is not None and len(raw) == op.size:
                row['on_disk_hex'] = raw.hex()
                if op.size == 4:
                    row['on_disk_u32'] = struct.unpack('<I', raw)[0]
                    value = struct.unpack('<f', raw)[0]
                    row['on_disk_f32'] = value if math.isfinite(value) else str(value)
                elif op.size == 8:
                    row['on_disk_u64'] = struct.unpack('<Q', raw)[0]
                    value = struct.unpack('<d', raw)[0]
                    row['on_disk_f64'] = value if math.isfinite(value) else str(value)
    return row


def instruction_row(ins, read):
    try:
        regs_read, regs_write = ins.regs_access()
    except cs.CsError:
        regs_read, regs_write = [], []
    mem = [
        memory_operand(ins, op, read)
        for op in ins.operands if op.type == x.X86_OP_MEM
    ]
    row = {
        'rva': ins.address,
        'hex': ins.bytes.hex(),
        'mnemonic': ins.mnemonic,
        'operands': ins.op_str,
        'regs_read': reg_names(ins, regs_read),
        'regs_write': reg_names(ins, regs_write),
        'memory_operands': mem,
        'is_call': bool(ins.group(cs.CS_GRP_CALL)),
        'is_jump': bool(ins.group(cs.CS_GRP_JUMP)),
        'is_ret': bool(ins.group(cs.CS_GRP_RET)),
    }
    if (ins.group(cs.CS_GRP_CALL) or ins.group(cs.CS_GRP_JUMP)) and ins.operands:
        first = ins.operands[0]
        if first.type == x.X86_OP_IMM:
            row['direct_target_rva'] = int(first.imm)
    return row


def site_is_authenticated_read(ins):
    matches = []
    for op in ins.operands:
        if op.type != x.X86_OP_MEM:
            continue
        if int(op.mem.disp) != v4.FIELD_OFFSET:
            continue
        if not (op.access & cs.CS_AC_READ):
            continue
        matches.append(op)
    return len(matches) == 1


def cfg_successors(ins, by_address, begin, end):
    fallthrough = ins.address + ins.size
    if ins.group(cs.CS_GRP_RET):
        return []
    if ins.group(cs.CS_GRP_JUMP):
        if not ins.operands or ins.operands[0].type != x.X86_OP_IMM:
            return []
        target = int(ins.operands[0].imm)
        rows = []
        if begin <= target < end and target in by_address:
            rows.append(target)
        if ins.id != x.X86_INS_JMP and begin <= fallthrough < end and fallthrough in by_address:
            rows.append(fallthrough)
        return list(dict.fromkeys(rows))
    if begin <= fallthrough < end and fallthrough in by_address:
        return [fallthrough]
    return []


def bounded_slice(update_info, site, read):
    instructions = update_info['instructions']
    by_address = {ins.address: ins for ins in instructions}
    site_ins = by_address.get(site)
    if site_ins is None:
        raise v4.Refusal(f'consumer_site_not_instruction_boundary_0x{site:x}')
    if not site_is_authenticated_read(site_ins):
        raise v4.Refusal(f'consumer_site_not_unique_disp250_read_0x{site:x}')

    ordered = [ins.address for ins in instructions]
    index = ordered.index(site)
    pre = instructions[max(0, index - MAX_PRE_CONTEXT):index]

    state = [site]
    queued = {site}
    visited = set()
    visits = 0
    reachable = []
    while state:
        address = state.pop(0)
        queued.discard(address)
        visits += 1
        if visits > MAX_WORKLIST_VISITS:
            raise v4.Refusal(f'consumer_cfg_visit_cap_0x{site:x}')
        if address in visited:
            continue
        if len(reachable) >= MAX_REACHABLE_INSTRUCTIONS:
            raise v4.Refusal(f'consumer_reachable_instruction_cap_0x{site:x}')
        visited.add(address)
        reachable.append(address)
        ins = by_address[address]
        for nxt in cfg_successors(
            ins, by_address, update_info['begin_rva'], update_info['end_rva']
        ):
            if nxt not in visited and nxt not in queued:
                state.append(nxt)
                queued.add(nxt)

    return {
        'site_rva': site,
        'site_instruction': instruction_row(site_ins, read),
        'pre_context': [instruction_row(ins, read) for ins in pre],
        'reachable_instruction_count': len(reachable),
        'worklist_visits': visits,
        'reachable_cfg_slice': [
            instruction_row(by_address[address], read)
            for address in sorted(reachable)
        ],
        'scope_note': 'Update-only CFG; direct calls are recorded but callees are not traversed',
    }


def scan(data):
    if len(data) > v4.MAX_FILE:
        raise v4.Refusal('file_cap')
    digest = hashlib.sha256(data).hexdigest()
    if digest != v4.EXPECTED_SHA:
        raise v4.Refusal('unsupported_build_sha256')

    _, image_size, sections = v4.peutil.pe_sections(data)
    runtimes, read = v4.parse_runtime_table(data, sections, image_size)
    runtime_by_begin = {row[0]: row for row in runtimes}
    md = v4.decoder()
    update_info, source_info, source_roots = v4.validate_source_relation(
        md, runtime_by_begin, read
    )

    sites = [bounded_slice(update_info, site, read) for site in READ_SITES]
    result = {
        'schema': SCHEMA,
        'result': 'WALLRUN_0X250_AUTHENTICATED_UPDATE_CONSUMER_SLICES',
        'exe_sha256': digest,
        'decoder': cs.__version__,
        'field_owner_offset': v4.FIELD_OFFSET,
        'authenticated_source': {
            'update': {
                'begin_rva': update_info['begin_rva'],
                'end_rva': update_info['end_rva'],
                'sha256': update_info['sha256'],
                'known_disp250_sites': list(READ_SITES),
            },
            'owner_callee': {
                'begin_rva': source_info['begin_rva'],
                'end_rva': source_info['end_rva'],
                'sha256': source_info['sha256'],
            },
            'pinned_roots': source_roots,
        },
        'consumer_sites': sites,
        'scope': {
            'update_only': True,
            'max_pre_context_instructions': MAX_PRE_CONTEXT,
            'max_reachable_instructions_per_site': MAX_REACHABLE_INSTRUCTIONS,
            'direct_callee_traversal': False,
            'global_executable_scan': False,
            'adjacent_offsets_scanned': False,
            'live_reads_or_writes': 0,
            'game_process_operation': False,
            'gameplay_write_authorized': False,
            'on_disk_rip_relative_values': 'INITIAL_FILE_BYTES_ONLY_NOT_RUNTIME_VALUES',
            'field_semantics': 'UNKNOWN',
        },
        'interpretation': (
            'bounded static consumer evidence for the two authenticated Update reads; '
            'RIP-relative values are on-disk initial bytes only and do not prove live '
            'runtime values, field semantics, producer identity, or safe writability'
        ),
    }
    payload = (json.dumps(result, indent=2) + '\n').encode()
    if len(payload) > MAX_OUTPUT:
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
            'schema': SCHEMA,
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
        f"{result['result']}: sites={len(result['consumer_sites'])} output={args.output}"
    )
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
