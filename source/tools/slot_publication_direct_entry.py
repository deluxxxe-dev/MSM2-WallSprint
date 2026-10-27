"""c55 OFFLINE exact direct-CALL entry proofs; no inferred function span/ABI.

Only the pinned frozen image is accepted by the CLI. Pure owned domain seams
are for tests. No process interface, data/qword values or indirect recovery.
"""
import argparse
from bisect import bisect_left, bisect_right
from collections import Counter
from dataclasses import asdict, dataclass
import hashlib
import heapq
import json
from pathlib import Path
import struct

import slot_publication_callee as c54

cfg = c54.cfg
EXPECTED_SHA = cfg.EXPECTED_SHA
VERSION = 'v0.1c55'
CONTRACT = 'direct_call_entry_normal_CFG_never_written_per_GPR_v1'
RELATIONS = ('exact_runtime_entry', 'inside_unique_runtime_span_nonentry',
    'overlapping_multiple_runtime_spans', 'executable_file_backed_outside_runtime_metadata',
    'executable_not_fully_file_backed', 'non_executable_or_writable_section',
    'outside_image', 'deferred_producer_excluded')


def digest(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


@dataclass(frozen=True)
class Caps(c54.SummaryCaps):
    diagnostic_targets: int = 65536
    diagnostic_callsites: int = 65536


class Domain(c54.ImageDomain):
    def __init__(self, **kw):
        super().__init__(**kw)
        self.intervals = tuple(sorted((b,e) for b,e,_ in kw['rows']))
        self.starts = tuple(b for b,_ in self.intervals)
        self.prefix_ends = tuple(self._prefix_ends())

    def _prefix_ends(self):
        high = 0
        for _, end in self.intervals:
            high = max(high,end)
            yield high

    def containing(self, pc):
        # At most two matches are needed to reject ambiguity. PE admission
        # independently rejects overlapping rows; this also handles owned tests.
        index = bisect_right(self.starts,pc)-1
        found = []
        while index >= 0 and self.prefix_ends[index] > pc:
            begin,end = self.intervals[index]
            if begin <= pc < end:
                found.append((begin,end))
                if len(found) == 2: break
            index -= 1
        return found

    def relation(self, pc):
        if not 0 <= pc < self.image_size: return 'outside_image'
        if any(b <= pc < e for b,e in self.excluded): return 'deferred_producer_excluded'
        rows = self.containing(pc)
        if len(rows) > 1: return 'overlapping_multiple_runtime_spans'
        if rows: return 'exact_runtime_entry' if rows[0][0]==pc else 'inside_unique_runtime_span_nonentry'
        sections = [s for s in self.sections if s[0] <= pc < s[1]]
        if len(sections)!=1 or sections[0][4] & 0x60000000 != 0x60000000 or sections[0][4] & 0x80000000:
            return 'non_executable_or_writable_section'
        if pc >= sections[0][0]+sections[0][3]: return 'executable_not_fully_file_backed'
        return 'executable_file_backed_outside_runtime_metadata'

    def window(self, pc):
        if self.relation(pc) != 'executable_file_backed_outside_runtime_metadata':
            raise ValueError('direct_edge_leaves_metadata_free_RX_domain')
        section = next(s for s in self.sections if s[0] <= pc < s[1])
        high = min(section[1], section[0]+section[3], pc+15)
        index = bisect_right(self.starts,pc)
        if index < len(self.starts): high = min(high,self.starts[index])
        for begin,_ in self.excluded:
            if pc < begin: high = min(high,begin)
        return high-pc

    def code_range(self, pc, size):
        if not 0 < size <= 15 or any(pc < e and b < pc+size for b,e in self.excluded):
            raise ValueError('invalid_or_deferred_callsite_bytes')
        sections = [s for s in self.sections if s[0] <= pc and pc+size <= min(s[1],s[0]+s[3])
                    and s[4] & 0x60000000 == 0x60000000 and not s[4] & 0x80000000]
        if len(sections)!=1: raise ValueError('callsite_not_unique_readonly_executable_file_backed')
        # Validation only; the caller meters the actual read before performing it.


class RuntimeEngine(c54.SummaryEngine):
    """Exact c54 admission/partition/effects/caps; only child routing is separate."""
    def child(self, ins, begin, end, depth):
        return self.router.get(ins.operands[0].imm,depth+1,
            edge=self.router.edge(ins,begin,c54.CONTRACT,end))


@dataclass(frozen=True)
class DirectSummary(c54.Summary):
    cfg_hash: str | None = None
    decoded_bytes: int = 0

    def record(self, image_id):
        row = super().record(image_id)
        row.update(contract=CONTRACT,end_rva=None,function_span_inferred=False,
            reachable_code_sha256=self.code_hash,reachable_CFG_sha256=self.cfg_hash,
            reachable_decoded_bytes=self.decoded_bytes if self.status=='ADMITTED' else None)
        return row


class Engine:
    def __init__(self, source, caps=Caps()):
        if any(n<=0 for n in asdict(caps).values()): raise ValueError('all summary caps must be positive')
        self.source,self.caps,self.image_id=source,caps,source.image_id
        runtime_caps=c54.SummaryCaps(**{k:getattr(caps,k) for k in asdict(c54.SummaryCaps())})
        self.runtime=RuntimeEngine(source,runtime_caps);self.runtime.router=self
        self.cache,self.by_entry,self.active={}, {}, set()
        self.metrics=Counter(dict.fromkeys(['direct_entry_summary_attempted','direct_entry_summary_admitted',
            'direct_entry_summary_refused','direct_entry_cache_hits','direct_entry_nested_summaries_used',
            'direct_entry_cycles_refused','direct_entry_entries_loaded','direct_entry_code_bytes_loaded',
            'direct_entry_instructions_decoded','direct_entry_work','summary_requests'],0))
        self.relation_requests=Counter(dict.fromkeys(RELATIONS,0))
        self.relation_callsites=Counter(dict.fromkeys(RELATIONS,0))
        self.targets,self.sites,self.callsites={},set(),[]
        self.refusal_examples={}
        self.decoded_owners,self.decoded_code={},{}
        self.refusals,self.admission_refusals=Counter(),Counter()
        self.exhausted=False
        self.diagnostics_complete=True

    @property
    def closed(self): return self.exhausted or self.runtime.exhausted

    def charge(self,n=1):
        if self.closed or n > self.caps.work-self.metrics['direct_entry_work']:
            self.exhausted=True
            raise ValueError('global_direct_entry_work_cap_or_exhausted')
        self.metrics['direct_entry_work']+=n

    def read(self,pc,n):
        if n > self.caps.bytes-self.metrics['direct_entry_code_bytes_loaded']:
            self.exhausted=True
            raise ValueError('global_direct_entry_byte_cap')
        self.charge(n)
        raw=bytes(self.source.read(pc,n))
        self.metrics['direct_entry_code_bytes_loaded']+=n
        if len(raw)!=n: raise ValueError('direct_entry_not_file_backed')
        return raw

    @staticmethod
    def edge(ins,begin,contract,end=None):
        return dict(site_rva=ins.address,instruction_size=ins.size,
            instruction_sha256=hashlib.sha256(bytes(ins.bytes)).hexdigest(),
            caller_entry_rva=begin,caller_end_rva=end,caller_contract=contract)

    def classify(self,target,edge):
        relation=self.source.relation(target)
        self.relation_requests[relation]+=1
        if target not in self.targets:
            if len(self.targets)>=self.caps.diagnostic_targets:
                self.exhausted=True
                self.diagnostics_complete=False
                raise ValueError('diagnostic_unique_target_cap')
            self.targets[target]=dict(target_rva=target,metadata_relation=relation,requests=0)
        self.targets[target]['requests']+=1
        if edge is not None:
            site=(edge['caller_contract'],edge['caller_entry_rva'],edge['site_rva'],target)
            if site not in self.sites:
                if len(self.sites)>=self.caps.diagnostic_callsites:
                    self.exhausted=True
                    self.diagnostics_complete=False
                    raise ValueError('diagnostic_unique_callsite_cap')
                self.sites.add(site);self.relation_callsites[relation]+=1
        return relation

    def verify_edge(self,target,edge):
        if edge is None: raise ValueError('missing_authenticated_direct_CALL_edge')
        if edge['caller_contract'] not in {'authenticated_runtime_caller_normal_CFG',c54.CONTRACT,CONTRACT}:
            raise ValueError('unrecognized_caller_contract')
        pc,n=edge['site_rva'],edge['instruction_size']
        begin=edge['caller_entry_rva'];contract=edge['caller_contract']
        if contract==CONTRACT:
            if begin not in self.active or self.decoded_code.get(pc)!=(n,edge['instruction_sha256']):
                raise ValueError('direct_caller_not_active_reached_instruction')
        else:
            if self.source.relation(begin)!='exact_runtime_entry':
                raise ValueError('caller_not_unique_authenticated_runtime_entry')
            span=self.source.describe(begin)
            if span!=(begin,edge['caller_end_rva']) or not begin<=pc<pc+n<=span[1]:
                raise ValueError('CALL_not_within_authenticated_caller_span')
            if contract==c54.CONTRACT and begin not in self.runtime.active:
                raise ValueError('runtime_caller_not_active_canonical_summary')
        # Validate code domain BEFORE reading, including producer exclusion.
        self.source.code_range(pc,n)
        raw=self.read(pc,n)
        self.charge()
        ins=next(cfg.old.decoder().disasm(raw,pc,count=1),None)
        if (ins is None or ins.size!=n or not ins.group(cfg.cs.CS_GRP_CALL)
            or len(ins.operands)!=1 or ins.operands[0].type!=cfg.x.X86_OP_IMM
            or ins.operands[0].imm!=target or hashlib.sha256(raw).hexdigest()!=edge['instruction_sha256']):
            raise ValueError('direct_CALL_edge_code_or_target_mismatch')

    def reject(self,target,reason,attempt=True):
        if attempt:
            self.metrics['direct_entry_summary_attempted']+=1
            self.metrics['direct_entry_summary_refused']+=1
            self.refusals[reason]+=1
        else:self.admission_refusals[reason]+=1
        if len(self.refusal_examples)<self.caps.refusal_examples:
            self.refusal_examples.setdefault((target,reason),dict(entry_rva=target,reason=reason))
        return DirectSummary(target,None,None,None,'REFUSED',reason)

    def get(self,target,depth=1,edge=None):
        self.metrics['summary_requests']+=1
        try:
            relation=self.classify(target,edge)  # always before fallback/cache
            self.charge()
            if self.source.image_id!=self.image_id: raise ValueError('summary_image_identity_changed')
            self.verify_edge(target,edge)
        except ValueError as error:return self.reject(target,str(error),False)
        if relation=='exact_runtime_entry':
            result=self.runtime.get(target,depth)
            return result
        if relation!='executable_file_backed_outside_runtime_metadata':
            return self.reject(target,'target_metadata_relation:'+relation,False)
        if depth>self.caps.depth:return self.reject(target,'direct_entry_depth_cap')
        if target in self.active:
            self.metrics['direct_entry_cycles_refused']+=1
            return self.reject(target,'recursive_direct_entry_cycle')
        cached=self.cache.get(self.by_entry.get(target))
        if cached is not None:
            if cached.status=='ADMITTED' and depth+cached.height-1>self.caps.depth:
                return self.reject(target,'cached_direct_entry_transitive_depth_cap')
            self.metrics['direct_entry_cache_hits']+=1
            return cached
        if self.metrics['direct_entry_entries_loaded']>=self.caps.functions:
            self.exhausted=True
            return self.reject(target,'global_direct_entry_function_cap')
        self.metrics['direct_entry_entries_loaded']+=1
        self.metrics['direct_entry_summary_attempted']+=1
        self.active.add(target)
        try:result=self.analyze(target,depth)
        except ValueError as error:
            reason=str(error)
            if reason=='direct_entry_control_cycle':self.metrics['direct_entry_cycles_refused']+=1
            result=DirectSummary(target,None,None,None,'REFUSED',reason)
        finally:self.active.remove(target)
        key=(self.image_id,target,CONTRACT,result.code_hash,result.cfg_hash,result.children)
        self.cache[key]=result;self.by_entry[target]=key
        self.metrics['direct_entry_summary_admitted' if result.status=='ADMITTED' else 'direct_entry_summary_refused']+=1
        if result.status!='ADMITTED':self.refusals[result.reason]+=1
        return result

    def analyze(self,entry,depth):
        md=cfg.old.decoder();nodes,edges,kinds={}, {}, {};starts=[]
        todo,queued=[entry],{entry};edge_count=0;decoded_bytes=0
        while todo:
            self.charge();pc=heapq.heappop(todo)
            if len(nodes)>=self.caps.instructions:raise ValueError('direct_entry_instruction_cap')
            if len(nodes)>=self.caps.states:raise ValueError('direct_entry_state_cap')
            window=self.source.window(pc)
            raw=self.read(pc,window)
            ins=next(md.disasm(raw,pc,count=1),None)
            if ins is None:raise ValueError('direct_entry_decode_gap_or_boundary')
            index=bisect_left(starts,pc)
            if (index and starts[index-1]+nodes[starts[index-1]].size>pc) or (index<len(starts) and pc+ins.size>starts[index]):
                raise ValueError('direct_entry_conflicting_instruction_intervals')
            if decoded_bytes+ins.size>self.caps.region_bytes:raise ValueError('direct_entry_decoded_byte_cap')
            self.charge(ins.size)  # global byte-ownership map is bounded by work
            signature=(ins.size,hashlib.sha256(bytes(ins.bytes)).hexdigest())
            if pc in self.decoded_code and self.decoded_code[pc]!=signature:
                raise ValueError('direct_entry_global_stale_decode')
            if any(self.decoded_owners.get(at,pc)!=pc for at in range(pc,pc+ins.size)):
                raise ValueError('direct_entry_cross_summary_conflicting_decode')
            self.decoded_code[pc]=signature
            for at in range(pc,pc+ins.size):self.decoded_owners[at]=pc
            starts.insert(index,pc);nodes[pc]=ins;decoded_bytes+=ins.size
            self.metrics['direct_entry_instructions_decoded']+=1
            following,kind=cfg.successors(ins,0,self.source.image_size,self.source.image_size)
            if kind=='return' and ins.operands:raise ValueError('unsupported_direct_entry_return_adjustment')
            following=list(dict(following).items());edges[pc],kinds[pc]=following,kind
            for target,_ in following:
                self.charge()
                if edge_count>=self.caps.edges:raise ValueError('direct_entry_edge_cap')
                edge_count+=1
                self.source.window(target)  # reject runtime/deferred/RW/unbacked before decoding
                if target not in queued:queued.add(target);heapq.heappush(todo,target)
        preds={pc:set() for pc in nodes}
        for pc,following in edges.items():
            for target,_ in following:preds[target].add(pc)
        blocks,previous=0,None
        for pc in sorted(nodes):
            self.charge()
            if previous is None or preds[pc]!={previous} or edges[previous]!=[(pc,'fallthrough')]:
                blocks+=1
                if blocks>self.caps.blocks:raise ValueError('direct_entry_block_cap')
            previous=pc
        indegree={pc:len(p) for pc,p in preds.items()};ready=[pc for pc,n in indegree.items() if not n];heapq.heapify(ready)
        order=[]
        while ready:
            self.charge();pc=heapq.heappop(ready);order.append(pc)
            for target,_ in edges[pc]:
                indegree[target]-=1
                if not indegree[target]:heapq.heappush(ready,target)
        if len(order)!=len(nodes):raise ValueError('direct_entry_control_cycle')
        if any(not edges[pc] and kinds[pc]!='return' for pc in nodes):raise ValueError('direct_entry_non_RET_exit')
        writes,children,height,returns=set(),set(),1,0
        for pc in order:
            self.charge();ins=nodes[pc];writes.update(c54.written_banks(ins))
            if kinds[pc]=='return':returns+=1
            if kinds[pc]=='call':
                child=self.get(ins.operands[0].imm,depth+1,edge=self.edge(ins,entry,CONTRACT))
                if child.status!='ADMITTED':raise ValueError('nested_summary_refused:'+child.reason)
                self.metrics['direct_entry_nested_summaries_used']+=1
                writes.update(set(range(16))-child.preserved)
                children.add(child.identity);height=max(height,1+child.height)
        if not returns:raise ValueError('direct_entry_has_no_normal_RET')
        for pc in sorted(nodes):
            self.charge();ins=nodes[pc]
            if self.read(pc,ins.size)!=bytes(ins.bytes):raise ValueError('direct_entry_stale_byte_mismatch')
        code_hash=digest([(pc,bytes(nodes[pc].bytes).hex()) for pc in sorted(nodes)])
        cfg_hash=digest([(pc,edges[pc]) for pc in sorted(nodes)])
        identity=digest([self.image_id,entry,CONTRACT,code_hash,cfg_hash,sorted(children)])
        return DirectSummary(entry,None,code_hash,identity,'ADMITTED','all_reached_normal_paths_RET_and_never_written',
            c54.NON_RSP-writes,tuple(sorted(children)),height,len(nodes),len(nodes),edge_count,blocks,returns,cfg_hash,decoded_bytes)

    def for_site(self,target,edge):
        summary=self.get(target,edge=edge)
        if len(self.callsites)<self.caps.callsite_examples:
            self.callsites.append(dict(edge,target_rva=target,metadata_relation=self.source.relation(target),
                **summary.certificate()))
        return summary.certificate()

    def observe_region(self,region):pass  # callsite examples captured with exact CALL evidence above

    def report(self):
        r=self.runtime.report()
        return dict(self.metrics,c54_runtime_entry_summary_attempted=r['callee_summaries_attempted'],
            c54_runtime_entry_summary_admitted=r['callee_summaries_admitted'],
            c54_runtime_entry_summary_refused=r['callee_summaries_refused'],
            c54_runtime_entry_analysis=r,direct_entry_refusal_reasons=dict(sorted(self.refusals.items())),
            target_admission_refusal_reasons=dict(sorted(self.admission_refusals.items())),
            refusal_examples=[row for _,row in sorted(self.refusal_examples.items())],
            target_metadata_relation_request_counts=dict(self.relation_requests),
            target_metadata_relation_unique_counts={k:sum(v['metadata_relation']==k for v in self.targets.values()) for k in RELATIONS},
            target_metadata_relation_unique_callsite_counts=dict(self.relation_callsites),
            unique_targets_reported=len(self.targets),unique_callsites_reported=len(self.sites),
            direct_entry_unique_targets_considered=sum(v['metadata_relation']=='executable_file_backed_outside_runtime_metadata' for v in self.targets.values()),
            target_frequency_examples=sorted(self.targets.values(),key=lambda v:(-v['requests'],v['target_rva']))[:self.caps.refusal_examples],
            callsite_examples=self.callsites,diagnostic_counts_complete=self.diagnostics_complete,
            direct_entry_summaries=[s.record(self.image_id) for s in sorted(self.cache.values(),key=lambda s:s.entry)],
            direct_entry_caps=asdict(self.caps),global_summary_budget_exhausted=self.closed,
            combined_summary_work=self.metrics['direct_entry_work']+r['summary_work'],
            combined_summary_work_cap=2*self.caps.work,
            global_direct_decode_partition_bytes=len(self.decoded_owners),
            global_direct_decode_partition_instructions=len(self.decoded_code),
            combined_summary_byte_cap=2*self.caps.bytes,combined_summary_entry_cap=2*self.caps.functions,
            scope='exact_CALL_edge_normal_reachable_CFG_not_function_span_or_EH_or_ownership',
            rsp_always_killed=True,ABI_preservation_assumed=False,return_value_origins_created=False)


def analyze_image(data,caps=cfg.Caps(),summary_caps=Caps()):
    if len(data)>cfg.old.MAX_FILE:raise ValueError('input file cap')
    data=bytes(data)
    if hashlib.sha256(data).hexdigest()!=EXPECTED_SHA:raise ValueError('exact executable SHA256 mismatch')
    result=cfg.analyze_image(data,caps,call_context_factory=lambda **kw:Engine(Domain(**kw),summary_caps))
    report=result.pop('callee_summary_analysis');result.update(report)
    result.update(tool_version=VERSION,schema_version=3,repository_result='DIRECT_CALL_ENTRY_SLOT_SCANNER_READY',
        direct_entry_contract=CONTRACT,c54_runtime_contract=c54.CONTRACT,
        call_edges_with_any_preserved_origin=result['coverage'].get('call_edges_with_any_preserved_origin',0),
        preserved_origin_register_facts_across_calls=result['coverage'].get('preserved_origin_register_facts_across_calls',0),
        call_clobber_all_address_origins_remaining=result['unresolved_memory_reasons'].get('call_clobber_all_address_origins',0),
        c53_call_clobber_all_address_origins_baseline=324714,
        total_instructions_decoded_including_summaries=result['coverage']['instructions_decoded']+report['direct_entry_instructions_decoded']+report['c54_runtime_entry_analysis']['summary_instructions_decoded'],
        total_code_bytes_loaded_including_summaries=result['coverage']['file_code_bytes_loaded']+report['direct_entry_code_bytes_loaded']+report['c54_runtime_entry_analysis']['summary_code_bytes_loaded'],
        real_image_effect_not_inferred_from_synthetic=True,
        reason='bounded_exact_direct_CALL_candidates_not_writer_or_publication_completeness')
    return result


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    try:
        if args.exe.stat().st_size>cfg.old.MAX_FILE:raise ValueError('input file cap')
        with args.exe.open('rb') as source:result=analyze_image(source.read(cfg.old.MAX_FILE+1))
        text=json.dumps(result,sort_keys=True,separators=(',',':'))+'\n'
        args.output.write_text(text,encoding='utf-8')
    except (OSError,ValueError,struct.error) as error:parser.error(str(error))
    print(text,end='')


if __name__=='__main__':main()
