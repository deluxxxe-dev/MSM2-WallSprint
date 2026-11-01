"""Pinned offline scan for direct executable references/writers to WallRun.this+0x250.

This is a code-reference discriminator only. A displacement match is not ownership or
field-semantic proof. No game process is opened, no memory is written, and no values
from the EXE data sections are exported.
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

MAX_CODE_BYTES = 192 * 1024 * 1024
MAX_RAW_DISP_HITS = 200_000
MAX_CANDIDATE_FUNCTIONS = 16_384
MAX_DECODE_FUNCTIONS = 16_384
MAX_FUNCTION_BYTES = 256 * 1024
MAX_TOTAL_DECODE_BYTES = 96 * 1024 * 1024
MAX_RETAINED_REFS = 1024
MAX_WRITERS = 512
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
    if not size or size % 12 or size > 8 * 1024 * 1024 or size // 12 > 600_000:
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


def scan(data):
    if len(data) > peutil.MAX_FILE:
        raise Refusal('file_cap')
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED_SHA:
        raise Refusal('unsupported_build_sha256')
    _, image_size, sections = peutil.pe_sections(data)
    runtimes, read = parse_runtime_table(data, sections, image_size)
    starts = [r[0] for r in runtimes]
    md = decoder()

    update_body = read(UPDATE_BEGIN, UPDATE_END - UPDATE_BEGIN)
    if hashlib.sha256(update_body).hexdigest() != UPDATE_SHA256:
        raise Refusal('authenticated_Update_hash_mismatch')
    for at, expected in KNOWN_CALL_PREFIXES.items():
        if read(at, len(expected)) != expected:
            raise Refusal('authenticated_Update_owner_call_prefix_mismatch')

    executable = [s for s in sections if s['size'] and s['flags'] & 0x20000000]
    if sum(s['size'] for s in executable) > MAX_CODE_BYTES:
        raise Refusal('code_scan_cap')

    # Byte hits only choose containing runtime functions. Instruction boundaries are
    # established later by one complete linear decode per candidate function.
    raw_hits = 0
    functions = set()
    needle = struct.pack('<I', FIELD_DISP)
    for sec in executable:
        blob = data[sec['raw']:sec['raw'] + sec['size']]
        pos = 0
        while True:
            hit = blob.find(needle, pos)
            if hit < 0:
                break
            pos = hit + 1
            raw_hits += 1
            if raw_hits > MAX_RAW_DISP_HITS:
                raise Refusal('raw_displacement_hit_cap')
            hit_rva = sec['rva'] + hit
            i = bisect_right(starts, hit_rva) - 1
            if i < 0:
                continue
            runtime = runtimes[i]
            if runtime[0] <= hit_rva < runtime[1]:
                functions.add(runtime[0])
                if len(functions) > MAX_CANDIDATE_FUNCTIONS:
                    raise Refusal('candidate_function_cap')

    if UPDATE_BEGIN not in functions:
        raise Refusal('authenticated_Update_not_in_disp250_prefilter')

    refs = []
    writers = []
    reference_count = 0
    unknown_access_count = 0
    decoded_bytes = 0
    decoded_functions = 0
    oversize_functions = 0
    noncontiguous_functions = 0
    known_sites = set()
    owner_stats = {
        target: {'disp250_reference_count': 0, 'disp250_writer_count': 0,
                 'memory_base_rcx_writer_count_without_path_provenance': 0}
        for target in KNOWN_OWNER_CALLS.values()
    }

    for begin in sorted(functions):
        if decoded_functions >= MAX_DECODE_FUNCTIONS:
            raise Refusal('decoded_function_cap')
        runtime = runtimes[bisect_right(starts, begin) - 1]
        if runtime[0] != begin:
            raise Refusal('runtime_index_inconsistent')
        end, unwind = runtime[1], runtime[2]
        size = end - begin
        if size > MAX_FUNCTION_BYTES:
            oversize_functions += 1
            continue
        decoded_bytes += size
        if decoded_bytes > MAX_TOTAL_DECODE_BYTES:
            raise Refusal('aggregate_decode_byte_cap')
        body = read(begin, size)
        decoded_functions += 1
        instructions = list(md.disasm(body, begin))
        if not instructions or instructions[0].address != begin:
            noncontiguous_functions += 1
            continue

        cursor = begin
        contiguous = True
        for ins in instructions:
            if ins.address != cursor:
                contiguous = False
                break
            cursor += ins.size
        if not contiguous or cursor != end:
            noncontiguous_functions += 1
            continue

        function_sha = hashlib.sha256(body).hexdigest()
        relation = ('authenticated_Update' if begin == UPDATE_BEGIN else
                    'direct_authenticated_Update_owner_rcx_callee_entry' if begin in KNOWN_OWNER_CALLS.values() else
                    'global_disp250_reference_function_NOT_owner_proven')

        for ins in instructions:
            mem = memory_rows(ins)
            if not mem:
                continue
            reference_count += 1
            if any(not m['access_known'] for m in mem):
                unknown_access_count += 1
            if ins.address in KNOWN_UPDATE_READS:
                known_sites.add(ins.address)
            is_writer = any(m['write'] for m in mem)

            if begin in owner_stats:
                owner_stats[begin]['disp250_reference_count'] += 1
                if is_writer:
                    owner_stats[begin]['disp250_writer_count'] += 1
                    owner_stats[begin]['memory_base_rcx_writer_count_without_path_provenance'] += sum(
                        1 for m in mem if m['write'] and m['base'] == 'rcx'
                    )

            retain = is_writer or ins.address in KNOWN_UPDATE_READS or begin in KNOWN_OWNER_CALLS.values()
            if not retain:
                continue

            row = {
                'instruction_rva': ins.address,
                'function_begin_rva': begin,
                'function_end_rva': end,
                'function_unwind_rva': unwind,
                'function_sha256': function_sha,
                'function_relation': relation,
                'mnemonic': ins.mnemonic,
                'operands': ins.op_str,
                'instruction_hex': ins.bytes.hex(),
                'memory_operands': mem,
                'instruction_boundary_proof': 'complete_PE_runtime_function_linear_decode',
                'entry_owner_rcx_relation': begin in KNOWN_OWNER_CALLS.values(),
                'memory_base_is_rcx_at_instruction': any(m['base'] == 'rcx' for m in mem),
                'wallrun_owner_proven_at_instruction': False,
                'field_semantics_proven': False,
            }
            refs.append(row)
            if len(refs) > MAX_RETAINED_REFS:
                raise Refusal('retained_reference_cap')
            if is_writer:
                writers.append(row)
                if len(writers) > MAX_WRITERS:
                    raise Refusal('writer_cap')

    found_sites = sorted(known_sites)
    if tuple(found_sites) != KNOWN_UPDATE_READS:
        raise Refusal('known_Update_disp250_site_selfcheck_failed')

    owner_callee_rows = []
    for call_site, target in KNOWN_OWNER_CALLS.items():
        stats = owner_stats[target]
        owner_callee_rows.append({
            'call_site_rva': call_site,
            'target_rva': target,
            'target_runtime_entry_seen': any(rt[0] == target for rt in runtimes),
            'target_was_prefilter_candidate': target in functions,
            'entry_rcx_is_authenticated_WallRun_owner': True,
            **stats,
            'owner_proven_at_writer_instruction': False,
        })

    result = {
        'schema': 2,
        'result': 'DIRECT_DISP250_WRITERS_FOUND' if writers else 'NO_DIRECT_DISP250_WRITER_IN_FIXED_SCAN',
        'exe_sha256': digest,
        'decoder': cs.__version__,
        'authenticated_update': {
            'begin_rva': UPDATE_BEGIN,
            'end_rva': UPDATE_END,
            'sha256': UPDATE_SHA256,
            'known_disp250_sites_required': list(KNOWN_UPDATE_READS),
            'known_disp250_sites_found': found_sites,
            'owner_rcx_direct_calls': owner_callee_rows,
        },
        'field_displacement': FIELD_DISP,
        'raw_displacement_hits': raw_hits,
        'candidate_runtime_functions': len(functions),
        'decoded_runtime_functions': decoded_functions,
        'decoded_function_bytes': decoded_bytes,
        'oversize_candidate_functions_skipped': oversize_functions,
        'noncontiguous_candidate_functions_skipped': noncontiguous_functions,
        'direct_reference_count': reference_count,
        'unknown_access_reference_count': unknown_access_count,
        'retained_reference_evidence': refs,
        'direct_writers': writers,
        'limits': {
            'global_scan': 'executable_file_bytes_prefilter_to_containing_PE_runtime_functions',
            'instruction_boundary': 'complete_PE_runtime_function_linear_decode_required',
            'computed_address_writes': 'NOT_COVERED',
            'bulk_copy_or_memcpy': 'NOT_COVERED_unless_decoded_instruction_itself_has_disp_0x250',
            'nested_pointer_fields': 'NOT_COVERED',
            'field_owner': 'NOT_PROVEN_by_displacement_or_entry_RCX_alone',
            'runtime_selection': 'NOT_PROVEN',
            'field_semantics': 'UNKNOWN',
            'live_reads_or_writes': 0,
            'game_process_operation': False,
        },
        'interpretation': 'direct x64 memory operands with displacement 0x250 only; byte hits only choose functions, and only full runtime-function decode establishes instruction references',
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
        result = {'schema': 2, 'result': 'REFUSED', 'reason': str(exc), 'live_reads_or_writes': 0, 'game_process_operation': False}
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
        print(f'REFUSED: {exc}')
        return 2
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(f"{result['result']}: writers={len(result['direct_writers'])} refs={result['direct_reference_count']} output={args.output}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
