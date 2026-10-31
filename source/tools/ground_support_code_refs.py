"""Pinned offline, targeted ground-family instruction references; never a game ABI."""
import argparse
from bisect import bisect_right
import hashlib
import json
from pathlib import Path
import re
import struct
import capstone as cs
from capstone import x86 as x
import discover_ground_support as meta

MAX_NAMES = 64
MAX_TARGETS = 8
MAX_DATA_REFS = 128
MAX_CODE_SCAN = 192*1024*1024
MAX_PREFILTER = 2_000_000
MAX_XREFS = 32
MAX_FUNCTIONS = 16
MAX_FUNCTION_HASH_BYTES = 2*1024*1024
MAX_PREFIX_BYTES = 4096
MAX_BOUNDARY_INSNS = 8192
MAX_EVIDENCE_BYTES = 512
MAX_OUTPUT = 128*1024
NEEDLES = ('HeroState', 'AutoUpdateGroundState', 'MoveAccelSprint',
           'StickConfigSprint', 'HeroSprintStartEvent', 'HeroSprintStopEvent')
EXCLUDE = re.compile(r'Wall|Air|Swim|Mount|Vault|Bot|Vehicle|Combat|Attack|Anim|Remote', re.I)
# REX.W + LEA/MOV with ModRM.mod=00,r/m=101: prefilter only, NOT instruction proof.
PREFILTER = re.compile(rb'(?=([\x48-\x4f][\x8d\x8b][\x05\x0d\x15\x1d\x25\x2d\x35\x3d]....))', re.S)


def rank(name):
    plain = re.sub(r'^\.\?A[VU]', '', name).removesuffix('@@')
    if EXCLUDE.search(plain):
        return None
    if re.fullmatch(r'HeroState(?:Ground(?:Run|Move|Movement|Locomotion|ed)?|Run(?:Move)?|Sprint|Move|Movement|Locomotion)(?:Local)?', plain):
        return 0, 'ground_or_generic_HeroState_family_not_authenticated_state'
    if plain == 'AutoUpdateGroundState':
        return 1, 'owner_observed_ground_state_label_not_controller_proof'
    if plain in ('MoveAccelSprint', 'StickConfigSprint'):
        return 2, 'owner_observed_input_config_label_secondary_only'
    if plain in ('HeroSprintStartEvent', 'HeroSprintStopEvent'):
        return 3, 'owner_observed_sprint_event_label_secondary_only'
    return None


def decoder():
    if cs.__version__ != '5.0.6':
        raise meta.Refusal('requires_Capstone_5.0.6')
    md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_64)
    md.detail = True
    return md


def readonly(s):
    return s['size'] and s['flags'] & 0x40000000 and not s['flags'] & 0xa0000000


def investigate(data, expected_sha=meta.EXPECTED_SHA):
    if len(data) > meta.MAX_FILE:
        raise meta.Refusal('file_cap')
    digest = hashlib.sha256(data).hexdigest()
    if digest != expected_sha:
        raise meta.Refusal('unsupported_build_sha256')
    base, image_size, sections = meta.pe_sections(data)
    md = decoder()
    def section(at, size):
        found = [s for s in sections if s['rva'] <= at and at+size <= s['rva']+s['size']]
        if len(found) != 1:
            raise meta.Refusal('range_not_uniquely_file_backed')
        return found[0]
    def read(at, size):
        s = section(at, size)
        off = s['raw']+at-s['rva']
        return data[off:off+size]
    names = {}
    skipped = 0
    metadata = [s for s in sections if readonly(s)]
    if sum(s['size'] for s in metadata) > meta.MAX_METADATA:
        raise meta.Refusal('metadata_scan_cap')
    for s in metadata:
        blob = read(s['rva'], s['size'])
        for wide in (False, True):
            for needle in NEEDLES:
                token = needle.encode('utf-16le' if wide else 'ascii')
                at = 0
                while True:
                    hit = blob.find(token, at)
                    if hit < 0:
                        break
                    at = hit+len(token)
                    found = meta.string_at(blob, hit, wide)
                    if not found:
                        continue
                    start, name = found
                    reason = rank(name)
                    if reason is None:
                        skipped += 1
                        continue
                    rva = s['rva']+start
                    names[rva] = dict(name=name, name_rva=rva, rank=reason[0], reason=reason[1])
                    if len(names) > MAX_NAMES:
                        raise meta.Refusal('relevant_name_cap')
    ordered = sorted(names.values(), key=lambda a: (a['rank'], a['name'], a['name_rva']))
    selected = ordered[:MAX_TARGETS]
    targets = {a['name_rva']: dict(name_rva=a['name_rva'], relation='direct_name') for a in selected}
    data_refs = []
    for s in metadata:
        blob = read(s['rva'], s['size'])
        for row in selected:
            target = row['name_rva']
            for fmt, value, align, kind in (('<I', target, 4, 'rva32_word'), ('<Q', base+target, 8, 'va64_word')):
                token = struct.pack(fmt, value); at = 0
                while True:
                    hit = blob.find(token, at)
                    if hit < 0:
                        break
                    at = hit+1; source = s['rva']+hit
                    if source % align:
                        continue
                    data_refs.append(dict(source_rva=source, target_rva=target, kind=kind))
                    if len(data_refs) > MAX_DATA_REFS:
                        raise meta.Refusal('data_reference_cap')
                    # One metadata edge only; not proof of RTTI/registry membership.
                    targets.setdefault(source, dict(name_rva=target, relation=kind+'_to_name'))
    pe = struct.unpack_from('<I', data, 60)[0]; op = pe+24
    if struct.unpack_from('<I', data, op+108)[0] < 4:
        raise meta.Refusal('exception_directory_not_declared')
    table, size = struct.unpack_from('<II', data, op+112+3*8)
    if not size or size % 12 or size > 8*1024*1024 or size//12 > 600000:
        raise meta.Refusal('runtime_table_missing_or_cap')
    table_section = section(table, size)
    if not readonly(table_section):
        raise meta.Refusal('runtime_table_not_readonly')
    runtimes = []; previous = 0
    for b, e, u in struct.iter_unpack('<III', read(table, size)):
        if not 0 < b < e <= image_size or b < previous or u % 4:
            raise meta.Refusal('invalid_runtime_table_order_or_range')
        s = section(b, e-b)
        if not s['flags'] & 0x20000000:
            raise meta.Refusal('runtime_function_not_executable')
        header = read(u, 4)
        version, flags = header[0] & 7, header[0] >> 3
        if version not in (1, 2) or flags & ~7 or (flags & 4 and flags & 3):
            raise meta.Refusal('invalid_unwind_header')
        # Validate readable unwind extent, but do not claim chain/handler semantics.
        unwind_size = 4+((header[2]+1)//2)*4+(12 if flags & 4 else 4 if flags & 3 else 0)
        read(u, unwind_size)
        runtimes.append((b, e, u)); previous = e
    starts = [r[0] for r in runtimes]
    code_sections = [s for s in sections if s['flags'] & 0x20000000 and s['size']]
    if sum(s['size'] for s in code_sections) > MAX_CODE_SCAN:
        raise meta.Refusal('targeted_code_scan_cap')
    refs = []; funcs = {}; prefilters = 0; budget = MAX_BOUNDARY_INSNS; exported = 0; hashed = 0
    def boundary(b, e, site):
        nonlocal budget
        if site-b >= MAX_PREFIX_BYTES:
            return 'candidate_prefix_byte_cap'
        cursor = b
        while cursor <= site:
            if budget <= 0:
                return 'candidate_aggregate_decode_cap'
            budget -= 1
            ins = next(md.disasm(read(cursor, min(15, e-cursor)), cursor, count=1), None)
            if not ins:
                return 'candidate_decode_gap'
            if cursor == site:
                return 'runtime_entry_straight_line_boundary'
            if cursor+ins.size > site:
                return 'candidate_overlaps_prior_instruction'
            if any(ins.group(g) for g in (cs.CS_GRP_JUMP, cs.CS_GRP_CALL, cs.CS_GRP_RET,
                                          cs.CS_GRP_INT, cs.CS_GRP_IRET)) or ins.mnemonic in ('ud2','syscall','sysret'):
                return 'candidate_prior_control_boundary'
            cursor += ins.size
        return 'candidate_unproven'
    for s in code_sections:
        blob = read(s['rva'], s['size'])
        for match in PREFILTER.finditer(blob):
            prefilters += 1
            if prefilters > MAX_PREFILTER:
                raise meta.Refusal('prefilter_work_cap')
            site = s['rva']+match.start()
            target = site+7+struct.unpack_from('<i', match.group(1), 3)[0]
            if target not in targets:
                continue
            ins = next(md.disasm(match.group(1), site, count=1), None)
            if not ins or ins.size != 7 or ins.id not in (x.X86_INS_LEA, x.X86_INS_MOV):
                continue
            if len(ins.operands) != 2 or ins.operands[1].type != x.X86_OP_MEM or ins.operands[1].mem.base != x.X86_REG_RIP:
                continue
            i = bisect_right(starts, site)-1
            runtime = runtimes[i] if i >= 0 and site+ins.size <= runtimes[i][1] else None
            proof = boundary(*runtime[:2], site) if runtime else 'candidate_no_runtime_coverage'
            # Refuse known interior coincidences, retain explicitly unproven candidates.
            if proof == 'candidate_overlaps_prior_instruction':
                continue
            if len(refs) >= MAX_XREFS:
                raise meta.Refusal('instruction_reference_cap')
            function = None
            if runtime:
                b, e, u = runtime
                if b not in funcs:
                    if len(funcs) >= MAX_FUNCTIONS:
                        raise meta.Refusal('function_cap')
                    hashed += e-b
                    if hashed > MAX_FUNCTION_HASH_BYTES:
                        raise meta.Refusal('function_hash_byte_cap')
                    funcs[b] = dict(begin_rva=b, end_rva=e, unwind_rva=u,
                                    sha256=hashlib.sha256(read(b, e-b)).hexdigest(),
                                    identity='exact_PE_runtime_range_not_semantic_controller',
                                    body_exported=False)
                function = b
            # Minimal actual-reference evidence; at most 4 follow-on instructions/48 bytes.
            limit = min(site+48, runtime[1] if runtime else s['rva']+s['size'])
            evidence = []; first_followup = None
            for following in md.disasm(read(site, limit-site), site, count=5):
                if exported+following.size > MAX_EVIDENCE_BYTES:
                    raise meta.Refusal('exported_code_cap')
                exported += following.size
                memory = [dict(base=following.reg_name(o.mem.base), index=following.reg_name(o.mem.index),
                               displacement=o.mem.disp, width=o.size, access=o.access)
                          for o in following.operands if o.type == x.X86_OP_MEM]
                if following.address != site and first_followup is None:
                    if following.id == x.X86_INS_CALL and following.operands and following.operands[0].type == x.X86_OP_IMM:
                        call_target = following.operands[0].imm
                        index = bisect_right(starts, call_target)-1
                        callee = runtimes[index] if index >= 0 and runtimes[index][0] == call_target else None
                        first_followup = dict(kind='direct_call_after_name_reference_not_argument_or_state_proof',
                                              site_rva=following.address, target_rva=call_target,
                                              exact_runtime_entry=callee is not None,
                                              runtime_range=list(callee) if callee else None)
                    elif memory:
                        first_followup = dict(kind='subsequent_memory_operand_not_owner_field_proof',
                                              site_rva=following.address, operands=memory)
                evidence.append(dict(rva=following.address, size=following.size,
                                     mnemonic=following.mnemonic, operands=following.op_str,
                                     hex=following.bytes.hex(), memory_operands=memory))
                if any(following.group(g) for g in (cs.CS_GRP_JUMP, cs.CS_GRP_CALL, cs.CS_GRP_RET,
                                                   cs.CS_GRP_INT, cs.CS_GRP_IRET)):
                    break
            body_avoided = False
            if runtime and site == runtime[0] and evidence and evidence[-1]['rva']+evidence[-1]['size'] == runtime[1]:
                # Even a tiny function must not turn a bounded excerpt into a full-body dump.
                exported -= evidence[-1]['size']
                evidence.pop()
                body_avoided = True
            refs.append(dict(instruction_rva=site, target_rva=target, **targets[target],
                             boundary_proof=proof, function_begin_rva=function, evidence=evidence,
                             first_local_followup=first_followup, full_body_byte_dump_avoided=body_avoided,
                             interpretation='local_decoded_reference_not_ground_support_consumer'))
    proven = sum(r['boundary_proof'] == 'runtime_entry_straight_line_boundary' for r in refs)
    result = dict(schema=1, result='CODE_REFERENCE_CANDIDATES' if refs else 'NO_CODE_RELATION_IN_FIXED_SCOPE',
                  exe_sha256=digest, decoder=cs.__version__, selected_targets=selected,
                  relevant_names=len(names), omitted_ranked_names=max(0,len(names)-MAX_TARGETS),
                  excluded_or_unrelated_occurrences=skipped, metadata_edges=data_refs,
                  references=refs, functions=list(funcs.values()), straight_line_boundary_refs=proven,
                  code_bytes_exported=exported, prefix_decode_budget_remaining=budget,
                  selection_ambiguous=True, selected_controller=None, registry_authenticated=False,
                  support_frame='UNKNOWN', gravity_frame='UNKNOWN', ownership_proven=False,
                  first_consumer_authenticated=False, live_reads=0,
                  limitation='fixed name family/one metadata edge/REX.W RIP LEA-MOV only; no indirect/other addressing or CFG continuation; candidates never certify ABI')
    if len((json.dumps(result,indent=2)+'\n').encode()) > MAX_OUTPUT:
        raise meta.Refusal('output_cap')
    return result


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('exe', type=Path); p.add_argument('--output', required=True, type=Path)
    args = p.parse_args()
    try:
        with args.exe.open('rb') as f:
            result = investigate(f.read(meta.MAX_FILE+1))
    except (OSError, ValueError, struct.error) as e:
        result = dict(schema=1,result='REFUSED',reason=str(e) if isinstance(e,meta.Refusal) else 'malformed_or_unavailable',
                      selected_controller=None,ownership_proven=False,live_reads=0)
    with args.output.open('x',encoding='utf-8') as f:
        json.dump(result,f,indent=2);f.write('\n')
    print(result['result'],args.output)
    return 0 if result['result']=='CODE_REFERENCE_CANDIDATES' else 2

if __name__=='__main__':
    raise SystemExit(main())
