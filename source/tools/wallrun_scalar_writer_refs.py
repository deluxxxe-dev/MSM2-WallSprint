"""Pinned offline inspection of WallRun Update and its two authenticated owner-RCX direct callees.

This is a narrow code-reference discriminator only. It does not launch or attach to the
game, and it never writes memory. Entry RCX relation does not by itself prove that a
later memory operand still uses the WallRun owner.
"""
import argparse
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
FIELD_DISP = 0x250
KNOWN_UPDATE_READS = (0x1424585, 0x142460E)
KNOWN_OWNER_CALLS = {
    0x14245C3: 0x367DD10,
    0x1424630: 0x29BD5F0,
}
KNOWN_CALL_PREFIXES = {
    0x14245C0: bytes.fromhex('48 8b ce e8 48 97 25 02'),
    0x142462D: bytes.fromhex('48 8b ce e8 bb 8f 59 01'),
}
TARGET_ENTRIES = (UPDATE_BEGIN, 0x367DD10, 0x29BD5F0)

MAX_FILE = peutil.MAX_FILE
MAX_RUNTIME_TABLE = 8 * 1024 * 1024
MAX_RUNTIME_ENTRIES = 600_000
MAX_FUNCTION_BYTES = 256 * 1024
MAX_FUNCTION_INSTRUCTIONS = 65_536
MAX_REFS_PER_FUNCTION = 512
MAX_CALLS_PER_FUNCTION = 2_048
MAX_CONTEXT_INSNS = 12
MAX_OUTPUT = 512 * 1024


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
    previous = 0
    for begin, end, unwind in struct.iter_unpack('<III', read(table, size)):
        if not 0 < begin < end <= image_size or begin < previous or unwind % 4:
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
        previous = end
    return rows, read


def memory_rows(ins):
    rows = []
    for op in ins.operands:
        if op.type != x.X86_OP_MEM or op.mem.disp != FIELD_DISP:
            continue
        rows.append({
            'base': ins.reg_name(op.mem.base) if op.mem.base else '',
            'index': ins.reg_name(op.mem.index) if op.mem.index else '',
            'scale': op.mem.scale,
            'displacement': op.mem.disp,
            'width': op.size,
            'read': bool(op.access & cs.CS_AC_READ),
            'write': bool(op.access & cs.CS_AC_WRITE),
            'access_known': bool(op.access),
        })
    return rows


def compact_instruction(ins):
    return {
        'rva': ins.address,
        'mnemonic': ins.mnemonic,
        'operands': ins.op_str,
        'hex': ins.bytes.hex(),
    }


def decode_exact_function(md, runtime_by_begin, read, begin):
    runtime = runtime_by_begin.get(begin)
    if runtime is None:
        raise Refusal(f'target_not_exact_runtime_entry_0x{begin:x}')
    _, end, unwind = runtime
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


def context_for(instructions, index, before=6, after=3):
    start = max(0, index - before)
    end = min(len(instructions), index + after + 1)
    rows = [compact_instruction(ins) for ins in instructions[start:end]]
    if len(rows) > MAX_CONTEXT_INSNS:
        rows = rows[:MAX_CONTEXT_INSNS]
    return rows


def inspect_function(info, owner_entry_relation):
    instructions = info['instructions']
    refs = []
    writers = []
    calls = []
    for index, ins in enumerate(instructions):
        mem = memory_rows(ins)
        if mem:
            if len(refs) >= MAX_REFS_PER_FUNCTION:
                raise Refusal(f'target_reference_cap_0x{info["begin_rva"]:x}')
            row = {
                'instruction_rva': ins.address,
                'mnemonic': ins.mnemonic,
                'operands': ins.op_str,
                'instruction_hex': ins.bytes.hex(),
                'memory_operands': mem,
                'entry_rcx_is_authenticated_WallRun_owner': owner_entry_relation,
                'owner_proven_at_instruction': False,
                'field_semantics_proven': False,
            }
            refs.append(row)
            if any(m['write'] for m in mem):
                writer = dict(row)
                writer['local_context'] = context_for(instructions, index)
                writers.append(writer)
        if ins.id == x.X86_INS_CALL and ins.operands and ins.operands[0].type == x.X86_OP_IMM:
            if len(calls) >= MAX_CALLS_PER_FUNCTION:
                raise Refusal(f'target_direct_call_cap_0x{info["begin_rva"]:x}')
            target = int(ins.operands[0].imm)
            calls.append({
                'call_site_rva': ins.address,
                'target_rva': target,
                'entry_rcx_is_authenticated_WallRun_owner': owner_entry_relation,
                'owner_forwarded_to_target_rcx': False,
                'owner_forwarding_semantics': 'UNKNOWN',
                'prelude': [compact_instruction(p) for p in instructions[max(0, index - 3):index + 1]],
            })
    return refs, writers, calls


def scan(data):
    if len(data) > MAX_FILE:
        raise Refusal('file_cap')
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED_SHA:
        raise Refusal('unsupported_build_sha256')

    _, image_size, sections = peutil.pe_sections(data)
    runtimes, read = parse_runtime_table(data, sections, image_size)
    runtime_by_begin = {row[0]: row for row in runtimes}
    md = decoder()

    update_body = read(UPDATE_BEGIN, UPDATE_END - UPDATE_BEGIN)
    if hashlib.sha256(update_body).hexdigest() != UPDATE_SHA256:
        raise Refusal('authenticated_Update_hash_mismatch')
    for at, expected in KNOWN_CALL_PREFIXES.items():
        if read(at, len(expected)) != expected:
            raise Refusal('authenticated_Update_owner_call_prefix_mismatch')

    decoded = {}
    for begin in TARGET_ENTRIES:
        decoded[begin] = decode_exact_function(md, runtime_by_begin, read, begin)
    if decoded[UPDATE_BEGIN]['end_rva'] != UPDATE_END:
        raise Refusal('authenticated_Update_runtime_range_mismatch')
    if decoded[UPDATE_BEGIN]['sha256'] != UPDATE_SHA256:
        raise Refusal('authenticated_Update_runtime_hash_mismatch')

    function_rows = []
    all_target_writers = []
    update_sites = []
    for begin in TARGET_ENTRIES:
        owner_entry = begin in KNOWN_OWNER_CALLS.values()
        refs, writers, calls = inspect_function(decoded[begin], owner_entry)
        if begin == UPDATE_BEGIN:
            update_sites = sorted(r['instruction_rva'] for r in refs if r['instruction_rva'] in KNOWN_UPDATE_READS)
            if tuple(update_sites) != KNOWN_UPDATE_READS:
                raise Refusal('known_Update_disp250_site_selfcheck_failed')
            if writers:
                raise Refusal('authenticated_Update_unexpected_disp250_writer')
        else:
            all_target_writers.extend(writers)
        function_rows.append({
            'begin_rva': begin,
            'end_rva': decoded[begin]['end_rva'],
            'unwind_rva': decoded[begin]['unwind_rva'],
            'size': decoded[begin]['size'],
            'sha256': decoded[begin]['sha256'],
            'relation': 'authenticated_WallRun_Update' if begin == UPDATE_BEGIN else 'authenticated_Update_direct_callee_entry_receives_WallRun_owner_in_RCX',
            'entry_rcx_is_authenticated_WallRun_owner': owner_entry,
            'disp250_references': refs,
            'disp250_direct_writers': writers,
            'direct_calls': calls,
            'complete_linear_decode': True,
        })

    owner_call_rows = []
    update_insns = {ins.address: ins for ins in decoded[UPDATE_BEGIN]['instructions']}
    for call_site, target in KNOWN_OWNER_CALLS.items():
        ins = update_insns.get(call_site)
        if ins is None or ins.id != x.X86_INS_CALL or not ins.operands or ins.operands[0].type != x.X86_OP_IMM or int(ins.operands[0].imm) != target:
            raise Refusal('authenticated_Update_direct_call_target_mismatch')
        owner_call_rows.append({
            'call_site_rva': call_site,
            'target_rva': target,
            'target_is_exact_runtime_entry': target in runtime_by_begin,
            'entry_rcx_is_authenticated_WallRun_owner': True,
        })

    result = {
        'schema': 3,
        'result': 'OWNER_CALLEE_DIRECT_WRITER_CANDIDATES' if all_target_writers else 'NO_DIRECT_DISP250_WRITER_IN_AUTHENTICATED_OWNER_CALLEES',
        'exe_sha256': digest,
        'decoder': cs.__version__,
        'field_displacement': FIELD_DISP,
        'authenticated_update': {
            'begin_rva': UPDATE_BEGIN,
            'end_rva': UPDATE_END,
            'sha256': UPDATE_SHA256,
            'known_disp250_sites_required': list(KNOWN_UPDATE_READS),
            'known_disp250_sites_found': update_sites,
            'owner_rcx_direct_calls': owner_call_rows,
        },
        'target_functions': function_rows,
        'owner_callee_direct_writer_count': len(all_target_writers),
        'scope': {
            'functions_decoded': [hex(v) for v in TARGET_ENTRIES],
            'global_executable_scan': False,
            'adjacent_offsets_scanned': False,
            'computed_address_writes': 'NOT_COVERED',
            'bulk_copy_or_memcpy': 'NOT_COVERED',
            'nested_pointer_fields': 'NOT_COVERED',
            'owner_at_writer_instruction': 'NOT_PROVEN_without_intra_function_provenance',
            'field_semantics': 'UNKNOWN',
            'live_reads_or_writes': 0,
            'game_process_operation': False,
        },
        'interpretation': 'only authenticated WallRun Update plus its two direct owner-RCX callee runtime functions; direct writer candidates remain non-writable until owner provenance is proved at the writer instruction',
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
            'schema': 3,
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
    print(f"{result['result']}: owner_callee_writers={result['owner_callee_direct_writer_count']} output={args.output}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
