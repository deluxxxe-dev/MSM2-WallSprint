"""c53 OFFLINE normal-CFG exact image-address candidates, never pointer values.

CLI accepts only the pinned private executable. Synthetic scan_region() is a
pure owned-buffer test seam, not an authentication bypass. Candidates are
conditional on one authenticated runtime entry and the enumerated normal edges;
alternate instruction entries/EH/indirect writers remain unproven.
"""
import argparse
from collections import Counter, deque
from dataclasses import asdict, dataclass, replace
import hashlib
import heapq
import json
from pathlib import Path
import struct

import capstone as cs
from capstone import x86 as x
import slot_publication as old

EXPECTED_SHA = old.EXPECTED_SHA
SLOT = old.SLOT
DEFERRED = old.DEFERRED
SCHEMA = 1
VERSION = 'v0.1c53'


@dataclass(frozen=True)
class Caps:
    bytes: int = 64 * 1024 * 1024
    instructions: int = 5_000_000
    edges: int = 10_000_000
    states: int = 5_000_000
    iterations: int = 20_000_000
    blocks: int = 2_000_000
    regions: int = 600_000
    references: int = 256
    region_bytes: int = 65536
    region_instructions: int = 1024
    region_edges: int = 2048
    region_states: int = 1024
    region_iterations: int = 8192
    region_blocks: int = 512
    hops: int = 8


@dataclass(frozen=True)
class Fact:
    rva: int
    # Identical numeric addresses reached by different derivations do not merge.
    chain: tuple


EMPTY = (None,) * 16
LOOPS = {'loop', 'loope', 'loopne', 'jrcxz', 'jecxz', 'jcxz'}
SYSTEM = {'syscall', 'sysenter', 'sysret', 'sysexit', 'ud2', 'hlt',
          'xbegin', 'xend', 'xabort', 'rsm'}
MOVES = {'movups', 'movupd', 'movaps', 'movapd', 'movss', 'movsd',
         'movdqa', 'movdqu', 'movntps', 'movntpd', 'movntdq'}
RMW = {'add', 'adc', 'sub', 'sbb', 'and', 'or', 'xor', 'inc', 'dec',
       'neg', 'not', 'xadd', 'cmpxchg', 'xchg', 'bts', 'btr', 'btc'}
STAT_KEYS = ('regions_considered regions_modeled regions_refused regions_excluded cfg_entries blocks_traversed '
             'instructions_decoded decoded_bytes file_code_bytes_loaded states_discovered '
             'edges_traversed direct_branch_edges conditional_fallthrough_edges fallthrough_edges '
             'call_fallthrough_all_facts_killed_edges direct_calls_all_facts_invalidated '
             'worklist_iterations merge_visits state_revisits unique_joins loop_back_edges '
             'conflicting_facts_discarded previously_control_lost_edges_safely_modeled '
             'control_edges_without_agreed_origin exact_address_operands unresolved_possible_writer_operands').split()


def statistics():
    return Counter(dict.fromkeys(STAT_KEYS, 0))


def address(ins, op, state, image_base, image_size, width, reasons=None):
    proxy = old.AddressState()
    proxy.origins = [f.rva if f else None for f in state]
    if reasons is not None:
        proxy.reasons = reasons
    result, proof, reason = old.resolve_address(ins, op, proxy, image_base, image_size, width)
    return result, proof, reason


def transfer(ins, state, image_base, image_size, hops):
    """No loads/constants/arithmetic/ABI summaries; sources are pre-state."""
    result = list(state)
    ops = ins.operands
    assigned = None
    dest = None
    try:
        _, writes = ins.regs_access()
    except cs.CsError as exc:
        raise ValueError('unknown_register_effects') from exc
    banks = {old.PARENT[r] for r in writes if r in old.PARENT}
    if len(ops) == 2 and ops[0].type == x.X86_OP_REG and ops[0].reg in old.FULL and ops[0].size == 8:
        dest = old.PARENT[ops[0].reg]
        if ins.id == x.X86_INS_LEA and ops[1].type == x.X86_OP_MEM:
            origin, _, error = address(ins, ops[1], state, image_base, image_size, 1)
            base = ops[1].mem.base
            previous = state[old.PARENT[base]] if base in old.FULL else None
            # No absolute immediate LEA materialization; only RIP or exact base.
            if not error and (base == x.X86_REG_RIP or previous):
                chain = previous.chain if previous else ()
                assigned = Fact(origin, chain + ((ins.address, 'rip_lea' if not previous else 'affine_lea', origin),))
        elif ins.id == x.X86_INS_MOV and ops[1].type == x.X86_OP_REG and ops[1].reg in old.FULL and ops[1].size == 8:
            previous = state[old.PARENT[ops[1].reg]]
            if previous:
                assigned = Fact(previous.rva, previous.chain + ((ins.address, 'copy64', previous.rva),))
    for bank in banks:
        result[bank] = None
    if assigned and dest is not None and banks == {dest} and len(assigned.chain) <= hops:
        result[dest] = assigned
    if ins.group(cs.CS_GRP_CALL):
        result = list(EMPTY)  # ALL address facts, including nonvolatile, killed.
    return tuple(result)


def transfer_reasons(ins, state, reasons, nxt):
    """Diagnostic labels only, never used to introduce an exact fact."""
    if ins.group(cs.CS_GRP_CALL):
        return ('call_clobber_all_address_origins',) * 16
    result = list(reasons)
    for reg in ins.regs_access()[1]:
        if reg not in old.PARENT:
            continue
        bank = old.PARENT[reg]
        if nxt[bank]:
            reason = 'exact_static_image_address'
        elif reg not in old.FULL:
            reason = 'partial_register_mutation'
        elif len(ins.operands) == 2 and ins.operands[1].type == x.X86_OP_MEM and ins.id != x.X86_INS_LEA:
            reason = 'memory_loaded_base'
        elif ins.id == x.X86_INS_MOV and len(ins.operands) == 2 and ins.operands[1].type == x.X86_OP_REG and ins.operands[1].reg in old.FULL:
            source = old.PARENT[ins.operands[1].reg]
            reason = 'provenance_hop_cap' if state[source] else reasons[source]
        else:
            reason = 'unsupported_register_mutation'
        result[bank] = reason
    return tuple(result)


def opcode(ins):
    """Bounded encoding direction check; EVEX and other maps are unsupported."""
    data = bytes(ins.bytes)
    i = 0
    while i < len(data) and (data[i] in (0x66, 0x67, 0xf0, 0xf2, 0xf3, 0x2e, 0x36, 0x3e, 0x26, 0x64, 0x65) or 0x40 <= data[i] <= 0x4f):
        i += 1
    if i == len(data):
        return None
    if data[i] == 0xc5 and i + 2 < len(data):
        return (1, data[i + 2])
    if data[i] == 0xc4 and i + 3 < len(data):
        return (data[i + 1] & 31, data[i + 3])
    if data[i] == 0x62:
        return None
    if data[i] == 0x0f and i + 1 < len(data):
        return (1, data[i + 1])
    return (0, data[i])


def access_kind(ins, index):
    """Unknown semantics refuse; decoder READ alone never proves read-only."""
    name = ins.mnemonic.removeprefix('lock ')
    if ins.id == x.X86_INS_LEA:
        return 'ADDRESS_ONLY', 'address_generation_only'
    encoding = opcode(ins)
    if name == 'mov' and len(ins.operands) == 2:
        stores = {(0, n) for n in (0x88, 0x89, 0xc6, 0xc7, 0xa2, 0xa3)}
        loads = {(0, n) for n in (0x8a, 0x8b, 0xa0, 0xa1)}
        if index == 0 and encoding in stores:
            return 'WRITE', 'encoding_verified_move_direction'
        if index == 1 and encoding in loads:
            return 'READ', 'encoding_verified_move_direction'
    family = name[1:] if name.startswith('v') else name
    if family in MOVES and len(ins.operands) == 2:
        if index == 0 and encoding in {(1, n) for n in (0x11, 0x29, 0x7f, 0x2b, 0xe7)}:
            return 'WRITE', 'encoding_verified_move_direction'
        if index == 1 and encoding in {(1, n) for n in (0x10, 0x28, 0x6f)}:
            return 'READ', 'encoding_verified_move_direction'
    if name in RMW and (index == 0 or name == 'xchg'):
        return 'RMW', 'explicit_RMW_instruction_semantics'
    if name in {'cmp', 'test', 'bt'}:
        return 'READ', 'explicit_read_only_instruction_semantics'
    return None, 'unsupported_access_semantics_possible_write'


def successors(ins, low, high, image_size, call_target_allowed=None):
    name = ins.mnemonic
    if name in SYSTEM or any(ins.group(g) for g in (cs.CS_GRP_INT, cs.CS_GRP_IRET, cs.CS_GRP_PRIVILEGE)):
        raise ValueError('system_or_unsupported_transfer')
    if ins.group(cs.CS_GRP_RET):
        if name != 'ret':
            raise ValueError('unsupported_return')
        return [], 'return'
    branch = ins.group(cs.CS_GRP_JUMP) or name in LOOPS
    call = ins.group(cs.CS_GRP_CALL)
    if branch or call:
        if len(ins.operands) != 1 or ins.operands[0].type != x.X86_OP_IMM:
            raise ValueError('indirect_call' if call else 'indirect_jump')
        target = ins.operands[0].imm
        if not 0 <= target < image_size:
            raise ValueError('unresolved_control_target')
        if call:
            if call_target_allowed is not None and not call_target_allowed(target):
                raise ValueError('unsupported_call_target_section')
            edges = [(ins.address + ins.size, 'call_fallthrough_all_facts_killed')]
        else:
            edges = [(target, 'direct_branch')]
            if name != 'jmp':
                edges.append((ins.address + ins.size, 'conditional_fallthrough'))
    else:
        edges = [(ins.address + ins.size, 'fallthrough')]
    if any(not low <= target < high for target, _ in edges):
        raise ValueError('unsupported_runtime_boundary')
    return edges, 'call' if call else 'branch' if branch else 'normal'


def scan_region(code, rva, slot=SLOT, image_base=0x140000000, image_size=0xD000000,
                caps=Caps(), excluded=(), call_target_allowed=None):
    """One entry, complete reachable normal CFG, then must fixpoint, THEN emit.

    Any unresolved reachable edge/decoding/graph or convergence cap rejects this
    whole region (no provisional candidates). Other runtime entries stay separate.
    """
    if any(v <= 0 for v in asdict(caps).values()):
        raise ValueError('all caps must be positive')
    md = old.decoder()
    stats = statistics()
    stats['regions_considered'] = 1
    stopped = Counter()
    unresolved = Counter()
    result = dict(entry_rva=rva, end_rva=rva + len(code), status='REFUSED', references=[],
                  stats={}, stopped_paths={}, unresolved_reasons={}, writer_enumeration_complete=False)

    def finish(reason=None):
        if reason:
            stopped[reason] += 1
        result.update(stats=dict(sorted(stats.items())), stopped_paths=dict(sorted(stopped.items())),
                      unresolved_reasons=dict(sorted(unresolved.items())))
        return result

    if not code or rva < 0 or rva + len(code) > image_size:
        return finish('invalid_runtime_span')
    if any(rva < end and begin < rva + len(code) for begin, end in excluded):
        return finish('deferred_region_excluded')
    if len(code) > min(caps.bytes, caps.region_bytes):
        return finish('region_byte_cap')
    stats['file_code_bytes_loaded'] = len(code)
    nodes, edges, kinds, decoded = {}, {}, {}, {}
    todo, queued = [rva], {rva}
    try:
        # Establish a single strict byte partition first, including unreachable
        # bytes. No resynchronization/alternate overlapping instruction streams.
        # This deliberately rejects some legitimate code/data-mixed regions.
        cursor = rva
        while cursor < rva + len(code):
            if len(decoded) >= min(caps.instructions, caps.region_instructions):
                raise ValueError('instruction_cap')
            ins = next(md.disasm(code[cursor-rva:cursor-rva+15], cursor, count=1), None)
            if ins is None:
                raise ValueError('decode_gap')
            decoded[cursor] = ins
            stats['instructions_decoded'] += 1
            stats['decoded_bytes'] += ins.size
            cursor += ins.size
        stats['cfg_entries'] = 1
        while todo:
            pc = heapq.heappop(todo)
            if pc in nodes:
                continue
            if pc not in decoded:
                raise ValueError('branch_into_middle_bytes')
            if len(nodes) >= min(caps.states, caps.region_states):
                raise ValueError('state_cap')
            ins = decoded[pc]
            # Validate explicit register-effect availability even on unused facts.
            transfer(ins, EMPTY, image_base, image_size, caps.hops)
            nodes[pc] = ins
            stats['states_discovered'] += 1
            following, kinds[pc] = successors(ins, rva, rva + len(code), image_size, call_target_allowed)
            edges[pc] = following
            for target, kind in following:
                if stats['edges_traversed'] >= min(caps.edges, caps.region_edges):
                    raise ValueError('edge_cap')
                stats['edges_traversed'] += 1
                stats[kind + '_edges'] += 1
                if target not in queued:
                    heapq.heappush(todo, target)
                    queued.add(target)
    except ValueError as error:
        return finish(str(error))

    predecessors = {pc: set() for pc in nodes}
    for pc, following in edges.items():
        for target, _ in following:
            predecessors[target].add(pc)
    blocks = {}
    previous = None
    for pc in sorted(nodes):
        if previous is None or predecessors[pc] != {previous} or edges[previous] != [(pc, 'fallthrough')]:
            if stats['blocks_traversed'] >= min(caps.blocks, caps.region_blocks):
                return finish('block_cap')
            block = pc
            stats['blocks_traversed'] += 1
        blocks[pc] = block
        previous = pc
    states = {rva: EMPTY}  # Other entries are UNREACHED, not known empty states.
    reasons = {rva: ('register_base_without_exact_origin',) * 16}
    todo = deque([rva])
    queued = {rva}
    try:
        while todo:
            if stats['worklist_iterations'] >= min(caps.iterations, caps.region_iterations):
                raise ValueError('iteration_cap')
            pc = todo.popleft()
            queued.remove(pc)
            stats['worklist_iterations'] += 1
            nxt = transfer(nodes[pc], states[pc], image_base, image_size, caps.hops)
            why = transfer_reasons(nodes[pc], states[pc], reasons[pc], nxt)
            for target, _ in edges[pc]:
                before = states.get(target)
                if before is None:
                    merged = nxt
                    merged_why = why
                else:
                    stats['merge_visits'] += 1
                    stats['state_revisits'] += 1
                    merged = tuple(a if a == b else None for a, b in zip(before, nxt))
                    merged_why = tuple('exact_static_image_address' if fact else
                        'cfg_merge_conflict' if before[i] != nxt[i] else
                        reasons[target][i] if reasons[target][i] == why[i] else 'cfg_merge_unknown'
                        for i, fact in enumerate(merged))
                    stats['conflicting_facts_discarded'] += sum(a != b for a, b in zip(before, nxt))
                if before != merged or reasons.get(target) != merged_why:
                    states[target] = merged
                    reasons[target] = merged_why
                    if target not in queued:
                        todo.append(target)
                        queued.add(target)
    except ValueError as error:
        return finish(str(error))
    assert len(states) == len(nodes)
    stats['unique_joins'] = sum(len(p) > 1 for p in predecessors.values())
    stats['loop_back_edges'] = sum(t <= pc for pc, e in edges.items() for t, _ in e)
    graph_hash = hashlib.sha256(json.dumps([(pc, edges[pc]) for pc in sorted(edges)], separators=(',', ':')).encode()).hexdigest()

    for pc in sorted(nodes):
        ins = nodes[pc]
        state = states[pc]
        nxt = transfer(ins, state, image_base, image_size, caps.hops)
        if kinds[pc] == 'call':
            stats['direct_calls_all_facts_invalidated'] += 1
        for target, kind in edges[pc]:
            if kind in ('direct_branch', 'conditional_fallthrough'):
                carried = any(a is not None and a == b for a, b in zip(nxt, states[target]))
                stats['previously_control_lost_edges_safely_modeled' if carried else 'control_edges_without_agreed_origin'] += 1
        for index, op in enumerate(ins.operands):
            if op.type != x.X86_OP_MEM:
                continue
            address_only = ins.id == x.X86_INS_LEA
            extent = 1 if address_only else op.size
            effective, proof, error = address(ins, op, state, image_base, image_size, extent, reasons[pc])
            if error:
                unresolved[error] += 1
                continue
            stats['exact_address_operands'] += 1
            if not effective < slot + 8 or not slot < effective + extent:
                continue
            operation, semantic_proof = access_kind(ins, index)
            if not operation:
                unresolved[semantic_proof] += 1
                stats['unresolved_possible_writer_operands'] += 1
                continue
            fact = state[old.PARENT[op.mem.base]] if op.mem.base in old.FULL else None
            low, high = max(effective, slot), min(effective + extent, slot + 8)
            if len(result['references']) >= caps.references:
                result['references'] = []
                return finish('reference_cap')
            result['references'].append(dict(site_rva=pc, instruction_sha256=hashlib.sha256(bytes(ins.bytes)).hexdigest(),
                mnemonic=ins.mnemonic, operands=ins.op_str, operand_index=index, operand_width=op.size,
                access_width=0 if address_only else op.size, operation=operation, may_write=operation in ('WRITE', 'RMW'),
                decoded_access=op.access, access_proof=semantic_proof, effective_rva=effective,
                overlap=dict(begin_rva=low, end_rva=high, bytes=high-low, address_point_only=address_only),
                address_proof_class='cfg_must_exact_static_image_address' if fact else proof,
                address_proof_chain=[list(step) for step in fact.chain] if fact else [],
                cfg=dict(entry_rva=rva, block_rva=blocks[pc], incoming_edges=len(predecessors[pc]),
                         incoming_predecessor_rvas=sorted(predecessors[pc]), graph_sha256=graph_hash,
                         region_reachable_nodes=len(nodes), region_joins=stats['unique_joins'],
                         region_loop_back_edges=stats['loop_back_edges'],
                         fixpoint_complete=True, address_fact_agrees_on_all_reachable_incoming_states=True,
                         scope='conditional_normal_CFG_from_single_runtime_entry_not_all_instruction_entries'),
                runtime_function=dict(begin_rva=rva, end_rva=rva+len(code)), section=None))
    result['status'] = 'MODELED_NORMAL_CFG'
    stats['regions_modeled'] = 1
    return finish()


def image_metadata(data):
    sections, base, excluded, file_range, size, _ = old.parse_pe(data)
    pe = struct.unpack_from('<I', data, 60)[0]
    opt = pe + 24
    names = {}
    start = opt + struct.unpack_from('<H', data, pe+20)[0]
    for i, section in enumerate(sections):
        names[section[0]] = data[start+40*i:start+40*i+8].split(b'\0')[0].decode('ascii', errors='replace')
    table, count = struct.unpack_from('<II', data, opt+112+3*8)
    if table < SLOT+8 and SLOT < table+count:
        raise ValueError('runtime metadata overlaps protected slot')
    rows = list(struct.iter_unpack('<III', file_range(table, count))) if count else []
    return sections, names, base, size, excluded, file_range, rows


def analyze_image(data, caps=Caps()):
    if any(v <= 0 for v in asdict(caps).values()):
        raise ValueError('all caps must be positive')
    if len(data) > old.MAX_FILE:
        raise ValueError('input file cap')
    if hashlib.sha256(data).hexdigest() != EXPECTED_SHA:
        raise ValueError('exact executable SHA256 mismatch')
    old.decoder()  # Pinned version enforced even if there is no admitted region.
    sections, names, base, size, excluded, file_range, rows = image_metadata(data)
    if not excluded:
        raise ValueError('deferred producer exclusion unavailable')
    slot_sections = [s for s in sections if s[0] <= SLOT and SLOT+8 <= s[1]]
    if len(slot_sections) != 1 or not slot_sections[0][4] & 0x40000000 or slot_sections[0][4] & 0x20000000:
        raise ValueError('slot must be unique readable non-executable image storage')
    stats, stopped, unresolved = statistics(), Counter(), Counter()
    references, anchors = [], []
    # Anchor is separately reauthenticated, never installed in ANY CFG entry.
    anchor_status = 'unavailable_or_unmatched'
    anchor_attempted = False
    if not any(old.ANCHOR_SITE < e and b < old.ANCHOR_SITE+7 for b, e in excluded):
        regions = [s for s in sections if s[0] <= old.ANCHOR_SITE and old.ANCHOR_SITE+7 <= s[1]
                   and s[4] & 0x60000000 == 0x60000000 and not s[4] & 0x80000000]
        if len(regions) == 1 and caps.bytes >= 7 and caps.instructions >= 1:
            try:
                raw = file_range(old.ANCHOR_SITE, 7)
            except ValueError:
                anchor_status = 'not_file_backed'
            else:
                anchor_attempted = True
                anchor = old.external_anchor_record(raw, base, size)
                stats['file_code_bytes_loaded'] += 7
                stats['instructions_decoded'] += 1
                stats['decoded_bytes'] += 7
                if anchor:
                    # The existing validator has proved this exact decoded
                    # MOV RAX,[RCX+0x100] form; do not decode a second time or
                    # introduce its accepted external origin into CFG states.
                    containing = [r for r in rows if r[0] <= old.ANCHOR_SITE and old.ANCHOR_SITE+7 <= r[1]]
                    anchor.update(operation='READ', operand_width=8, access_width=8,
                        instruction_sha256=hashlib.sha256(raw).hexdigest(), mnemonic='mov',
                        operands='rax, qword ptr [rcx + 0x100]',
                        overlap=dict(begin_rva=SLOT, end_rva=SLOT+8, bytes=8, address_point_only=False),
                        section=dict(name=names[regions[0][0]], begin_rva=regions[0][0], end_rva=regions[0][1]),
                        effective_section=dict(name=names[slot_sections[0][0]], begin_rva=slot_sections[0][0], end_rva=slot_sections[0][1]),
                        runtime_function=(dict(begin_rva=containing[0][0], end_rva=containing[0][1], unwind_rva=containing[0][2]) if containing else None),
                        cfg=dict(entry_rva=None, block_rva=None, fixpoint_complete=None,
                                 scope='authenticated_external_c28_anchor_not_local_CFG'))
                    anchors.append(anchor)
                    anchor_status = 'authenticated_external_c28_not_local_CFG_coverage'
    else:
        anchor_status = 'excluded_not_decoded'
    limits = {'bytes':'file_code_bytes_loaded', 'instructions':'instructions_decoded',
              'edges':'edges_traversed', 'states':'states_discovered',
              'iterations':'worklist_iterations', 'blocks':'blocks_traversed'}
    for begin, end, unwind in rows:
        if len(references) + len(anchors) >= caps.references:
            stopped['reference_cap'] += 1
            break
        if stats['regions_considered'] >= caps.regions:
            stopped['global_region_cap'] += 1
            break
        stats['regions_considered'] += 1
        if any(begin < e and b < end for b, e in excluded):
            stopped['deferred_region_excluded'] += 1
            stats['regions_excluded'] += 1
            continue
        region_sections = [s for s in sections if s[0] <= begin and end <= s[1]
            and s[4] & 0x60000000 == 0x60000000 and not s[4] & 0x80000000]
        if len(region_sections) != 1:
            stopped['unsupported_runtime_section'] += 1
            stats['regions_refused'] += 1
            continue
        if end-begin > caps.region_bytes:
            stopped['region_byte_cap'] += 1
            stats['regions_refused'] += 1
            continue
        remaining = {k:getattr(caps, k)-stats[v] for k, v in limits.items()}
        exhausted = [k for k, v in remaining.items() if v <= 0 or k == 'bytes' and v < end-begin]
        if exhausted:
            stopped['global_' + exhausted[0] + '_cap'] += 1
            stats['regions_refused'] += 1
            break
        try:
            code = file_range(begin, end-begin)
        except ValueError:
            stopped['runtime_span_not_file_backed'] += 1
            stats['regions_refused'] += 1
            continue
        region = scan_region(code, begin, image_base=base, image_size=size,
                             caps=replace(caps, **remaining, references=caps.references-len(references)-len(anchors)), excluded=excluded,
                             call_target_allowed=lambda pc: any(b <= pc < min(e, b+raw_size) and
                                 flags & 0x60000000 == 0x60000000 and not flags & 0x80000000
                                 for b, e, _, raw_size, flags in sections))
        stats.update({k:v for k, v in region['stats'].items() if k != 'regions_considered'})
        stopped.update(region['stopped_paths'])
        unresolved.update(region['unresolved_reasons'])
        if region['status'] != 'MODELED_NORMAL_CFG':
            stats['regions_refused'] += 1
        for ref in region['references']:
            ref['section'] = dict(name=names[region_sections[0][0]], begin_rva=region_sections[0][0], end_rva=region_sections[0][1])
            ref['runtime_function']['unwind_rva'] = unwind
            storage = [s for s in sections if s[0] <= ref['effective_rva'] and
                       ref['effective_rva'] + max(1, ref['access_width']) <= s[1]]
            ref['effective_section'] = (dict(name=names[storage[0][0]], begin_rva=storage[0][0], end_rva=storage[0][1])
                                        if len(storage) == 1 else None)
        references.extend(region['references'][:max(0, caps.references-len(references)-len(anchors))])
        if len(references)+len(anchors) >= caps.references:
            stopped['reference_cap'] += 1
            break
    modeled_edges = stats['previously_control_lost_edges_safely_modeled']
    return dict(tool_version=VERSION, schema_version=SCHEMA, repository_result='CFG_SLOT_PUBLICATION_SCANNER_READY',
        evidence='OFFLINE_STATIC', image_authenticated=True, image_sha256=EXPECTED_SHA,
        slot_rva=SLOT, slot_metadata=dict(readable=True, writable=bool(slot_sections[0][4] & 0x80000000)),
        scan_scope='bounded_single_runtime_entry_normal_CFG_must_address_candidates_not_all_writers',
        caps=asdict(caps), runtime_functions_in_metadata=len(rows), coverage=dict(sorted(stats.items())),
        runtime_functions_not_considered=len(rows)-stats['regions_considered'],
        executable_file_bytes=sum(min(e-b, n) for b, e, _, n, flags in sections if flags & 0x20000000),
        runtime_metadata_span_bytes=sum(e-b for b, e, _ in rows), executable_gaps_not_scanned=True,
        stopped_paths=dict(sorted(stopped.items())), unresolved_memory_reasons=dict(sorted(unresolved.items())),
        stopped_path_counting='first_refusal_per_affected_region_or_global_stop_not_all_possible_paths',
        anchor_instruction_budget_includes_one_validation_attempt=anchor_attempted,
        unresolved_memory_operands=sum(unresolved.values()), reference_candidates=references,
        reader_candidates=sum(r['operation']=='READ' for r in references),
        writer_candidates=sum(r['may_write'] for r in references),
        rmw_candidates=sum(r['operation']=='RMW' for r in references),
        address_only_candidates=sum(r['operation']=='ADDRESS_ONLY' for r in references),
        external_anchors=anchors, external_anchor_status=anchor_status, anchor_counted_as_local_candidate=False,
        excluded_deferred_regions=excluded, deferred_producer_rva=DEFERRED,
        deferred_exclusion_policy='c38_full_runtime_span_or_containing_executable_section_fallback_before_decode',
        legacy_comparison=dict(accepted_c38_control_flow_boundary_loss=822382,
            accepted_c38_runtime_region_boundary_loss=208548, accepted_c38_memory_loaded_base=85282,
            previously_lost_control_states_safely_modeled=modeled_edges,
            still_without_agreed_origin=stats['control_edges_without_agreed_origin'],
            refused_control_regions={k:v for k, v in sorted(stopped.items()) if any(
                s in k for s in ('indirect', 'transfer', 'runtime_boundary', 'middle', 'control_target', 'call_target'))},
            unit='direct_CFG_edges_with_agreed_origin_not_c38_operand_or_reset_totals',
            comparable_unresolved_categories=dict(sorted(unresolved.items())),
            same_region_legacy_run=False, historical_c38_counts_not_subtracted=True),
        candidate_sweep_complete=False, writer_enumeration_complete=False, publication_source_kind='unproven',
        allocation_proven=False, base_proven=False, lifetime_proven=False, range_proven=False,
        ownership_exclusive=False, destination_contained=False, CFG_continuation=False,
        alternate_entries_EH_indirect_writers_closed=False, live_qword_reads=0, pointer_chase=False,
        implicit_memory_operands_enumerated=False,
        production_integrated=False, hardware_watch_enabled=False, game_used=False,
        c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
        reason='bounded_normal_CFG_candidates_not_writer_or_publication_completeness')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    try:
        if args.exe.stat().st_size > old.MAX_FILE:
            raise ValueError('input file cap')
        with args.exe.open('rb') as source:
            result = analyze_image(source.read(old.MAX_FILE + 1))
        text = json.dumps(result, sort_keys=True, separators=(',', ':')) + '\n'
        args.output.write_text(text, encoding='utf-8')
    except (OSError, ValueError, struct.error) as error:
        parser.error(str(error))
    print(text, end='')


if __name__ == '__main__':
    main()
