"""c54 OFFLINE per-GPR never-written direct-callee summaries. No ABI/value rule.

CLI accepts only the pinned image. Summary providers for owned byte fixtures are
pure test seams; no CLI bypass, process access, stack/value or pointer chase.
"""
import argparse
from collections import Counter
from dataclasses import asdict, dataclass
import hashlib
import heapq
import json
from pathlib import Path
import struct
from types import MappingProxyType

import capstone as cs
from capstone import x86 as x
import slot_publication_cfg as cfg

EXPECTED_SHA = cfg.EXPECTED_SHA
VERSION = 'v0.1c54'
CONTRACT = 'normal_RET_never_written_per_GPR_v1'
NAMES = ('rax','rcx','rdx','rbx','rsp','rbp','rsi','rdi') + tuple('r'+str(i) for i in range(8,16))
NON_RSP = frozenset(range(16)) - {4}


@dataclass(frozen=True)
class SummaryCaps:
    depth: int = 8
    functions: int = 2048
    bytes: int = 8 * 1024 * 1024
    work: int = 1_000_000
    region_bytes: int = 16384
    instructions: int = 512
    blocks: int = 256
    edges: int = 1024
    states: int = 512
    refusal_examples: int = 64
    callsite_examples: int = 128


class ImageDomain:
    """Metadata/read closure is supplied ONLY after cfg exact-image admission."""
    def __init__(self, image_id, rows, sections, image_size, excluded, read):
        self.image_id, self.image_size = image_id, image_size
        self.excluded = tuple(tuple(r) for r in excluded)
        self.rows = MappingProxyType({b:(b,e) for b,e,_ in rows})
        self.sections, self.read = tuple(tuple(r) for r in sections), read

    def describe(self, target):
        span = self.rows.get(target)
        if span is None:
            raise ValueError('target_not_exact_authenticated_runtime_entry')
        begin, end = span
        if any(begin < e and b < end for b,e in self.excluded):
            raise ValueError('deferred_producer_excluded')
        containing = [s for s in self.sections if s[0] <= begin and end <= s[1]
            and end-s[0] <= s[3] and s[4] & 0x60000000 == 0x60000000 and not s[4] & 0x80000000]
        if len(containing) != 1:
            raise ValueError('callee_span_not_readonly_executable_file_backed')
        return span

    def load(self, span):
        return self.read(span[0], span[1]-span[0])


class OwnedDomain:
    """Immutable synthetic fixtures, never reachable from CLI image admission."""
    def __init__(self, functions, excluded=(), image_size=0xD000000):
        self.functions = MappingProxyType({pc:bytes(raw) for pc,raw in functions.items()})
        self.image_id = 'OWNED-' + hashlib.sha256(b''.join(
            struct.pack('<Q', pc)+hashlib.sha256(raw).digest() for pc,raw in sorted(self.functions.items()))).hexdigest()
        self.excluded, self.image_size = tuple(excluded), image_size
        self.loads = 0

    def describe(self, target):
        raw = self.functions.get(target)
        if raw is None:
            raise ValueError('target_not_exact_authenticated_runtime_entry')
        span = target, target+len(raw)
        if any(span[0] < e and b < span[1] for b,e in self.excluded):
            raise ValueError('deferred_producer_excluded')
        if not raw or not 0 <= span[0] < span[1] <= self.image_size or any(
                pc != target and span[0] < pc+len(code) and pc < span[1] for pc,code in self.functions.items()):
            raise ValueError('ambiguous_or_invalid_callee_span')
        return span

    def load(self, span):
        self.loads += 1
        return self.functions[span[0]]


# Only known effects, not arbitrary decoder metadata, may support PRESERVED.
INTEGER = {'mov','movzx','movsx','movsxd','lea','add','adc','sub','sbb','and','or','xor',
           'cmp','test','inc','dec','neg','not','shl','sal','shr','sar','rol','ror','rcl','rcr',
           'shld','shrd','bswap','bt','bts','btr','btc','xchg','xadd','push','pop','leave',
           'nop','endbr64','clc','stc','cmc','cld','std'}
VECTOR = cfg.MOVES | {'movd','movq','xorps','xorpd','andps','andpd','orps','orpd',
                     'addps','addpd','addss','addsd','subps','subpd','mulps','mulpd',
                     'pxor','pand','por','vzeroupper','vzeroall'}
READ_ONLY = {'cmp','test','bt','push','nop','endbr64','clc','stc','cmc','cld','std'}


def written_banks(ins):
    """Known explicit/implicit GPR write superset; no restored-value reasoning."""
    name = ins.mnemonic.removeprefix('lock ')
    control = ins.group(cs.CS_GRP_JUMP) or ins.group(cs.CS_GRP_CALL) or ins.group(cs.CS_GRP_RET) or name in cfg.LOOPS
    family = name[1:] if name.startswith('v') else name
    if not (control or name in INTEGER or name in VECTOR or family in VECTOR or
            name.startswith(('cmov', 'set'))):
        raise ValueError('unsupported_instruction_effect')
    for op in ins.operands:
        if op.type == x.X86_OP_MEM and (ins.addr_size != 8 or op.mem.index != x.X86_REG_INVALID or op.mem.segment != x.X86_REG_INVALID):
            raise ValueError('unsupported_callee_memory_addressing')
    try:
        _, writes = ins.regs_access()
    except cs.CsError as error:
        raise ValueError('unknown_register_effects') from error
    result = {cfg.old.PARENT[reg] for reg in writes if reg in cfg.old.PARENT}
    if not control and name not in READ_ONLY and ins.operands and ins.operands[0].type == x.X86_OP_REG:
        bank = cfg.old.PARENT.get(ins.operands[0].reg)
        if bank is not None:
            result.add(bank)  # include partial and conditional destination writes
    if ins.group(cs.CS_GRP_CALL) or ins.group(cs.CS_GRP_RET) or name in {'push','pop','leave'}:
        result.add(4)
    if name == 'leave':
        result.add(5)
    if name in {'loop','loope','loopne'}:
        result.add(1)
    if name in {'xchg','xadd'}:
        result.update(cfg.old.PARENT[op.reg] for op in ins.operands if op.type == x.X86_OP_REG and op.reg in cfg.old.PARENT)
    return frozenset(result)


@dataclass(frozen=True)
class Summary:
    entry: int
    end: int | None
    code_hash: str | None
    identity: str | None
    status: str
    reason: str
    preserved: frozenset = frozenset()
    children: tuple = ()
    height: int = 1
    instructions: int = 0
    states: int = 0
    edges: int = 0
    blocks: int = 0
    returns: int = 0

    def certificate(self):
        return dict(summary_id=self.identity, status=self.status, refusal_reason=None if self.status=='ADMITTED' else self.reason,
                    preserved_banks=sorted(self.preserved))

    def record(self, image_id):
        return dict(image_identity=image_id, entry_rva=self.entry, end_rva=self.end, code_sha256=self.code_hash,
            summary_id=self.identity, contract=CONTRACT, status=self.status, reason=self.reason,
            preserved_registers=[NAMES[i] for i in sorted(self.preserved)],
            register_results={name:('ALWAYS_KILLED' if i==4 else 'PRESERVED' if i in self.preserved else
                'WRITTEN_NOT_PRESERVED' if self.status=='ADMITTED' else 'UNPROVEN') for i,name in enumerate(NAMES)},
            nested_summary_ids=list(self.children), transitive_depth=self.height if self.status=='ADMITTED' else None,
            canonical_instructions=self.instructions if self.status=='ADMITTED' else None,
            reachable_states=self.states if self.status=='ADMITTED' else None,
            edges=self.edges if self.status=='ADMITTED' else None, blocks=self.blocks if self.status=='ADMITTED' else None,
            normal_returns=self.returns if self.status=='ADMITTED' else None)


class SummaryEngine:
    def __init__(self, source, caps=SummaryCaps()):
        if any(n <= 0 for n in asdict(caps).values()):
            raise ValueError('summary caps must be positive')
        self.source, self.caps = source, caps
        self.image_id = source.image_id
        self.cache, self.by_entry, self.active = {}, {}, set()
        self.metrics = Counter(dict.fromkeys(['summary_requests','callee_summaries_attempted','callee_summaries_admitted',
            'callee_summaries_refused','nested_summaries_used','summary_cycles_refused','summary_cache_hits',
            'summary_functions_loaded','summary_code_bytes_loaded','summary_work','summary_instructions_decoded'],0))
        self.refusals, self.examples, self.callsites = Counter(), {}, []
        self.dependencies = {}
        self.exhausted = False

    def charge(self, n=1):
        if n > self.caps.work-self.metrics['summary_work']:
            self.exhausted = True
            raise ValueError('global_summary_work_cap')
        self.metrics['summary_work'] += n

    def reject(self, target, reason):
        self.metrics['callee_summaries_attempted'] += 1
        self.metrics['callee_summaries_refused'] += 1
        self.refusals[reason] += 1
        if len(self.examples) < self.caps.refusal_examples:
            self.examples.setdefault((target,reason), dict(entry_rva=target,reason=reason,code_sha256=None))
        return Summary(target,None,None,None,'REFUSED',reason)

    def get(self, target, depth=1):
        self.metrics['summary_requests'] += 1
        if self.source.image_id != self.image_id:
            return self.reject(target,'summary_image_identity_changed')
        if self.exhausted:
            return self.reject(target,'global_summary_budget_already_exhausted')
        try:
            self.charge()  # cache/metadata lookup requests also consume work
        except ValueError as error:
            return self.reject(target,str(error))
        if depth > self.caps.depth:
            return self.reject(target,'callee_depth_cap')
        if target in self.active:
            self.metrics['summary_cycles_refused'] += 1
            return self.reject(target,'recursive_summary_cycle')
        cached = self.cache.get(self.by_entry.get(target))
        if cached is not None:
            if cached.status == 'ADMITTED' and depth+cached.height-1 > self.caps.depth:
                return self.reject(target,'cached_transitive_depth_cap')
            self.metrics['summary_cache_hits'] += 1
            return cached
        try:
            begin, end = self.source.describe(target)
            if self.metrics['summary_functions_loaded'] >= self.caps.functions:
                self.exhausted = True
                raise ValueError('global_summary_function_cap')
            if end-begin > self.caps.region_bytes:
                raise ValueError('callee_byte_cap')
            if end-begin > self.caps.bytes-self.metrics['summary_code_bytes_loaded']:
                self.exhausted = True
                raise ValueError('global_summary_byte_cap')
            self.charge(end-begin)  # reserve load/hash work BEFORE touching body
            code = bytes(self.source.load((begin,end)))
            if len(code) != end-begin:
                raise ValueError('callee_span_not_file_backed')
        except ValueError as error:
            return self.reject(target,str(error))
        self.metrics['summary_functions_loaded'] += 1
        self.metrics['summary_code_bytes_loaded'] += len(code)
        self.metrics['callee_summaries_attempted'] += 1
        code_hash = hashlib.sha256(code).hexdigest()
        identity = hashlib.sha256(json.dumps([self.source.image_id,begin,end,code_hash,CONTRACT],separators=(',',':')).encode()).hexdigest()
        self.active.add(target)
        try:
            result = self.analyze(code, begin, end, code_hash, identity, depth)
        except ValueError as error:
            reason = str(error)
            if reason == 'callee_control_cycle':
                self.metrics['summary_cycles_refused'] += 1
            result = Summary(begin,end,code_hash,identity,'REFUSED',reason,
                             children=tuple(sorted(self.dependencies.get(begin, ()))))
        finally:
            self.active.remove(target)
        key = (self.image_id,begin,end,code_hash,CONTRACT)
        self.cache[key] = result
        self.by_entry[target] = key
        self.metrics['callee_summaries_admitted' if result.status=='ADMITTED' else 'callee_summaries_refused'] += 1
        if result.status != 'ADMITTED':
            self.refusals[result.reason] += 1
        return result

    def analyze(self, code, begin, end, code_hash, identity, depth):
        md, decoded, cursor = cfg.old.decoder(), {}, begin
        while cursor < end:
            self.charge()
            if len(decoded) >= self.caps.instructions:
                raise ValueError('callee_instruction_cap')
            ins = next(md.disasm(code[cursor-begin:cursor-begin+15],cursor,count=1),None)
            if ins is None:
                raise ValueError('callee_decode_gap')
            decoded[cursor] = ins
            self.metrics['summary_instructions_decoded'] += 1
            cursor += ins.size
        nodes, edges, kinds = {}, {}, {}
        todo, queued, edge_count = [begin], {begin}, 0
        while todo:
            self.charge()
            pc = heapq.heappop(todo)
            if pc not in decoded:
                raise ValueError('callee_middle_entry')
            if len(nodes) >= self.caps.states:
                raise ValueError('callee_state_cap')
            ins = decoded[pc]
            following, kind = cfg.successors(ins,begin,end,self.source.image_size)
            following = list(dict(following).items())  # one CFG edge per target
            if kind == 'return' and ins.operands:
                raise ValueError('unsupported_callee_return_adjustment')
            nodes[pc], edges[pc], kinds[pc] = ins, following, kind
            for target, _ in following:
                self.charge()
                if edge_count >= self.caps.edges:
                    raise ValueError('callee_edge_cap')
                edge_count += 1
                if target not in queued:
                    queued.add(target);heapq.heappush(todo,target)
        preds = {pc:set() for pc in nodes}
        for pc, following in edges.items():
            for target,_ in following: preds[target].add(pc)
        blocks, previous = 0, None
        for pc in sorted(nodes):
            if previous is None or preds[pc] != {previous} or edges[previous] != [(pc,'fallthrough')]:
                blocks += 1
                if blocks > self.caps.blocks:
                    raise ValueError('callee_block_cap')
            previous = pc
        # Every reachable path must be finite. No loop-count/value reasoning.
        indegree = {pc:len(p) for pc,p in preds.items()}
        ready = [pc for pc,n in indegree.items() if n==0];heapq.heapify(ready)
        order = []
        while ready:
            self.charge();pc=heapq.heappop(ready);order.append(pc)
            for target,_ in edges[pc]:
                indegree[target] -= 1
                if not indegree[target]:heapq.heappush(ready,target)
        if len(order) != len(nodes):
            raise ValueError('callee_control_cycle')
        if any(not edges[pc] and kinds[pc]!='return' for pc in nodes):
            raise ValueError('callee_non_RET_exit')
        writes, children, height, returns = set(), set(), 1, 0
        for pc in order:
            self.charge();ins=nodes[pc]
            writes.update(written_banks(ins))
            if kinds[pc] == 'return': returns += 1
            if kinds[pc] == 'call':
                child = self.get(ins.operands[0].imm,depth+1)
                if child.identity is not None:
                    self.dependencies.setdefault(begin,set()).add(child.identity)
                if child.status != 'ADMITTED':
                    raise ValueError('nested_summary_refused:'+child.reason)
                self.metrics['nested_summaries_used'] += 1
                writes.update(set(range(16))-child.preserved)
                children.add(child.identity);height=max(height,1+child.height)
        if not returns:
            raise ValueError('callee_has_no_normal_RET')
        return Summary(begin,end,code_hash,identity,'ADMITTED','all_normal_paths_RET_and_per_register_never_written',
                       NON_RSP-writes,tuple(sorted(children)),height,len(decoded),len(nodes),edge_count,blocks,returns)

    def for_call(self, target):
        return self.get(target).certificate()

    def observe_region(self, region):
        for row in region.get('call_preservation_evidence', ()):
            if len(self.callsites) < self.caps.callsite_examples:
                self.callsites.append(dict(row,caller_entry_rva=region['entry_rva']))

    def report(self):
        return dict(self.metrics, summary_caps=asdict(self.caps), global_summary_budget_exhausted=self.exhausted,
            callee_summary_refusal_reasons=dict(sorted(self.refusals.items())),
            summaries=[s.record(self.image_id) for s in sorted(self.cache.values(),key=lambda s:s.entry)],
            refusal_examples=[r for _,r in sorted(self.examples.items())], callsite_examples=self.callsites,
            cache_identity='immutable_image_SHA_entry_span_code_SHA_contract',
            rsp_always_killed=True, ABI_preservation_assumed=False, return_value_origins_created=False,
            summary_scope='canonical_single_entry_acyclic_normal_RET_paths_not_EH_or_alternate_entries')


def analyze_image(data, caps=cfg.Caps(), summary_caps=SummaryCaps()):
    if len(data) > cfg.old.MAX_FILE:
        raise ValueError('input file cap')
    data = bytes(data)  # Freeze even a bytearray supplied to the pure unit seam.
    if len(data) > cfg.old.MAX_FILE or hashlib.sha256(data).hexdigest() != EXPECTED_SHA:
        raise ValueError('exact executable SHA256 mismatch or file cap')
    result = cfg.analyze_image(data,caps,call_context_factory=lambda **kw: SummaryEngine(ImageDomain(**kw),summary_caps))
    report = result.pop('callee_summary_analysis')
    result.update(report)
    result.update(tool_version=VERSION,schema_version=2,repository_result='CALLEE_PRESERVATION_SLOT_SCANNER_READY',
        callee_preservation_contract=CONTRACT,c53_call_clobber_all_address_origins_baseline=324714,
        call_edges_with_any_preserved_origin=result['coverage'].get('call_edges_with_any_preserved_origin',0),
        preserved_origin_register_facts_across_calls=result['coverage'].get('preserved_origin_register_facts_across_calls',0),
        call_clobber_all_address_origins_remaining=result['unresolved_memory_reasons'].get('call_clobber_all_address_origins',0),
        total_instructions_decoded_including_summaries=result['coverage']['instructions_decoded']+report['summary_instructions_decoded'],
        total_code_bytes_loaded_including_summaries=result['coverage']['file_code_bytes_loaded']+report['summary_code_bytes_loaded'],
        comparison_scope='same_operand_reason_category_new_scan_not_complete_or_identical_coverage',
        reason='bounded_never_written_direct_callee_address_candidates_not_writer_or_publication_completeness')
    return result


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    try:
        if args.exe.stat().st_size > cfg.old.MAX_FILE: raise ValueError('input file cap')
        with args.exe.open('rb') as source: result=analyze_image(source.read(cfg.old.MAX_FILE+1))
        text=json.dumps(result,sort_keys=True,separators=(',',':'))+'\n'
        args.output.write_text(text,encoding='utf-8')
    except (OSError,ValueError,struct.error) as error: parser.error(str(error))
    print(text,end='')


if __name__=='__main__':main()
