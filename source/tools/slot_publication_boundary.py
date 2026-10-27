"""c57 OFFLINE diagnostic classification of c56's first runtime-boundary refusal.

No cross-row CFG/value propagation or candidate admission. The CLI accepts only
the pinned image and accepted c56 set cardinality. Owned test seams are not CLI
authentication/count overrides. Optional c56 checkpoints are read-only inputs.
"""
import argparse
from bisect import bisect_left
from collections import Counter
from dataclasses import dataclass
import heapq
from pathlib import Path
import sys

import slot_publication_sharded as c56

cfg=c56.cfg
EXPECTED_SHA=c56.EXPECTED_SHA
VERSION='v0.1c57'
SCHEMA=1
CONTRACT='exact_first_c56_runtime_boundary_successors_diagnostic_v1'
READY='RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY'
BLOCKED='RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED'
KINDS=('ordinary_fallthrough','direct_unconditional_branch','direct_conditional_taken',
       'conditional_fallthrough','call_fallthrough','other_supported_direct_successor')
RELATIONS=('exact_unique_runtime_begin','inside_unique_runtime_span_nonentry',
    'overlapping_multiple_runtime_spans','executable_file_backed_outside_runtime_metadata',
    'executable_not_fully_file_backed','non_executable_or_writable_section','outside_image','deferred_producer_excluded')
INVALID=RELATIONS[2:3]+RELATIONS[4:]


@dataclass(frozen=True)
class Acceptance:
    metadata_rows:int=527161
    boundary_rows:int=162472
    reference_candidates:int=0
    reader_candidates:int=0
    writer_candidates:int=0
    rmw_candidates:int=0
    address_only_candidates:int=0


ACCEPTED_C56=Acceptance()


@dataclass(frozen=True)
class Witness:
    index:int
    metadata_sha256:str
    body_sha256:str
    work_sha256:str


@dataclass(frozen=True)
class Baseline:
    session:c56.Session
    witnesses:tuple
    result:dict
    acceptance:Acceptance


def source_identity():
    return dict(c56.source_identity(),slot_publication_boundary_py=c56.sha(Path(__file__).read_bytes()))


def checkpoint_parts(session,cache):
    """Complete existing c56 evidence only. Never create, repair or delete it."""
    cache=Path(cache)
    if cache.is_symlink() or not cache.is_dir():raise ValueError('c56 checkpoint directory unavailable')
    if c56.read_json(cache/'manifest.json')!=session.manifest:
        raise ValueError('stale/wrong image/tool/schema/contracts c56 manifest')
    expected={'manifest.json'}|{f'shard-{b:06d}-{e:06d}.json' for b,e in session.plan}
    for path in cache.iterdir():
        if path.name not in expected and not (path.name.startswith('.c56-') and path.suffix=='.tmp' and path.is_file() and not path.is_symlink()):
            raise ValueError('unexpected c56 checkpoint path')
    for start,end in session.plan:
        path=cache/f'shard-{start:06d}-{end:06d}.json'
        yield c56.read_json(path)  # missing/corrupt evidence refuses, no fallback


def collect_baseline(session,cache=None,acceptance=None,progress=None):
    acceptance=ACCEPTED_C56 if acceptance is None else acceptance
    if len(session.rows)!=acceptance.metadata_rows:raise ValueError('accepted c56 metadata count mismatch')
    aggregate=c56.Aggregate(session);witnesses=[]
    parts=checkpoint_parts(session,cache) if cache is not None else (session.make_shard(i) for i in range(len(session.plan)))
    for part in parts:
        aggregate.add(part)
        for row in part['payload']['rows']:
            if row['reason']=='unsupported_runtime_boundary':
                if row['status']!='REFUSED':raise ValueError('boundary set contains non-refused row')
                witnesses.append(Witness(row['index'],row['metadata_row_sha256'],row['body_code_sha256'],c56.digest(row['work'])))
        if progress is not None and (aggregate.count%32==0 or aggregate.count==len(session.plan)):
            progress('c56 baseline',aggregate.count,len(session.plan))
    result=aggregate.finish()
    if result['repository_result']!=c56.READY or result['tool_version']!=c56.VERSION or result['schema_version']!=c56.SCHEMA:
        raise ValueError('wrong c56 result identity')
    if (result['runtime_functions_not_considered']!=0 or len(witnesses)!=acceptance.boundary_rows
        or result['row_refusal_reasons'].get('unsupported_runtime_boundary',0)!=acceptance.boundary_rows):
        raise ValueError('accepted c56 boundary-refusal set/count mismatch')
    for key,expected in [('reference_candidate_count',acceptance.reference_candidates),
            ('reader_candidates',acceptance.reader_candidates),('writer_candidates',acceptance.writer_candidates),
            ('rmw_candidates',acceptance.rmw_candidates),('address_only_candidates',acceptance.address_only_candidates)]:
        if result[key]!=expected:raise ValueError('accepted c56 candidate counts changed')
    if any(result[k] for k in c56.FALSE_GATES):raise ValueError('baseline completeness/publication gates changed')
    return Baseline(session,tuple(witnesses),result,acceptance)


def diagnostic_successors(ins):
    """Use only AFTER the original oracle reports unsupported_runtime_boundary.

    CALL's c53 normal successor is its return-site fallthrough, not its callee.
    Conditional taken/fallthrough edges remain distinct even at equal targets.
    """
    branch=ins.group(cfg.cs.CS_GRP_JUMP) or ins.mnemonic in cfg.LOOPS
    call=ins.group(cfg.cs.CS_GRP_CALL)
    if call:return [('call_fallthrough',ins.address+ins.size,True,None)]
    if branch:
        if len(ins.operands)!=1 or ins.operands[0].type!=cfg.x.X86_OP_IMM:
            raise ValueError('unexpected non-direct boundary successor')
        target=ins.operands[0].imm
        if ins.id==cfg.x.X86_INS_JMP:
            result=[('direct_unconditional_branch',target,True,None)]
            if ins.mnemonic!='jmp':
                # Example: Capstone5.0.6 decorates MPX-prefixed JMP as 'bnd jmp'.
                # The unchanged c53 name test then adds a synthetic fallthrough.
                # Preserve its refusal cause without calling this architectural.
                result.append(('other_supported_direct_successor',ins.address+ins.size,False,
                    'existing_c53_decorated_JMP_model_only_fallthrough'))
            return result
        if ins.mnemonic=='jmp':raise ValueError('decoder JMP identity disagreement')
        return [('direct_conditional_taken',target,True,None),('conditional_fallthrough',ins.address+ins.size,True,None)]
    if ins.group(cfg.cs.CS_GRP_RET):raise ValueError('RET cannot be a c53 boundary refusal')
    return [('ordinary_fallthrough',ins.address+ins.size,True,None)]


def target_relation(session,source_begin,source_end,target):
    relation=session.domain.relation(target)
    if relation=='exact_runtime_entry':relation='exact_unique_runtime_begin'
    if relation not in RELATIONS:raise ValueError('unknown target metadata relation')
    row_index=begin=end=None;overlap=adjacent=None
    if relation=='exact_unique_runtime_begin':
        row_index=bisect_left(session.domain.starts,target)
        if row_index>=len(session.rows) or session.rows[row_index][0]!=target:
            raise ValueError('inconsistent unique target metadata identity')
        begin,end,_=session.rows[row_index]
        overlap=source_begin<end and begin<source_end
        adjacent=source_end==begin
    return dict(target_metadata_relation=relation,target_runtime_row_index=row_index,
        target_begin_rva=begin,target_end_rva=end,source_target_spans_overlap=overlap,exact_adjacency=adjacent)


def recover(session,witness):
    """Replay the exact original canonical partition/heap/transfer/edge order.

    No worklist successor is enqueued across the refused row and no target code
    is read. Every unexpected earlier refusal or work mismatch stops c57.
    """
    index=witness.index
    if type(index)!=int or not 0<=index<len(session.rows) or witness.metadata_sha256!=session.row_identity(index):
        raise ValueError('wrong source row identity/order')
    reason,code,_=session.preflight(index)
    if reason or code is None or c56.sha(code)!=witness.body_sha256:raise ValueError('stale/excluded source bytes')
    begin,end,_=session.rows[index];caps=session.profile.main
    work={k:0 for k in c56.WORK};work['file_code_bytes_loaded']=len(code)
    decoded={};cursor=begin;md=cfg.old.decoder()
    while cursor<end:
        if len(decoded)>=min(caps.instructions,caps.region_instructions):raise ValueError('earlier instruction_cap, not boundary')
        ins=next(md.disasm(code[cursor-begin:cursor-begin+15],cursor,count=1),None)
        if ins is None:raise ValueError('earlier decode_gap, not boundary')
        decoded[cursor]=ins;cursor+=ins.size
        work['instructions_decoded']+=1;work['decoded_bytes']+=ins.size
    nodes=set();todo=[begin];queued={begin}
    call_allowed=lambda pc:any(b<=pc<min(e,b+n) and flags&0x60000000==0x60000000 and not flags&0x80000000
        for b,e,_,n,flags in session.sections)
    while todo:
        pc=heapq.heappop(todo)
        if pc in nodes:continue
        if pc not in decoded:raise ValueError('earlier branch_into_middle_bytes, not boundary')
        if len(nodes)>=min(caps.states,caps.region_states):raise ValueError('earlier state_cap, not boundary')
        ins=decoded[pc]
        cfg.transfer(ins,cfg.EMPTY,session.base,session.size,caps.hops)
        nodes.add(pc);work['states_discovered']+=1
        try:following,_=cfg.successors(ins,begin,end,session.size,call_allowed)
        except ValueError as error:
            if str(error)!='unsupported_runtime_boundary':raise ValueError('earlier non-boundary refusal:'+str(error)) from error
            if c56.digest(work)!=witness.work_sha256:raise ValueError('first-refusal recovery work mismatch')
            edges=[]
            for kind,target,architectural,note in diagnostic_successors(ins):
                if not begin<=target<end:
                    edges.append(dict(edge_kind=kind,edge_target_rva=target,architectural_successor=architectural,
                        diagnostic_note=note,**target_relation(session,begin,end,target)))
            if not 1<=len(edges)<=2:raise ValueError('boundary refusal has no exact outgoing successors')
            return dict(source_runtime_row_index=index,source_begin_rva=begin,source_end_rva=end,
                source_runtime_row_sha256=witness.metadata_sha256,source_code_sha256=witness.body_sha256,
                source_instruction_rva=pc,source_instruction_size=ins.size,source_instruction_sha256=c56.sha(bytes(ins.bytes)),
                source_mnemonic=ins.mnemonic,
                first_refusal='unsupported_runtime_boundary',recovery_work=work,edges=edges)
        for target,_ in following:
            if work['edges_traversed']>=min(caps.edges,caps.region_edges):raise ValueError('earlier edge_cap, not boundary')
            work['edges_traversed']+=1
            if target not in queued:heapq.heappush(todo,target);queued.add(target)
    raise ValueError('expected source has no runtime-boundary refusal')


class Classifier:
    def __init__(self,baseline,example_limit=8):
        if type(example_limit)!=int or not 0<=example_limit<=8:raise ValueError('invalid example bound')
        self.baseline,self.limit=baseline,example_limit
        self.sources=source_identity()
        self.header=dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=baseline.session.header['image_sha256'],decoder_version=cfg.cs.__version__,
            c56_manifest_id=baseline.session.manifest['manifest_id'],tool_sources=self.sources,
            boundary_set_sha256=c56.digest([[w.index,w.metadata_sha256,w.body_sha256,w.work_sha256] for w in baseline.witnesses]),
            boundary_rows_expected=baseline.acceptance.boundary_rows,examples_per_class=example_limit)
        self.identity=c56.digest(self.header)
        if len(baseline.witnesses)!=baseline.acceptance.boundary_rows or any(
            a.index>=b.index for a,b in zip(baseline.witnesses,baseline.witnesses[1:])):
            raise ValueError('missing/duplicate/out-of-order accepted source set')

    def assert_current(self):
        self.baseline.session.assert_current_tool()
        if source_identity()!=self.sources:raise ValueError('classifier source changed during scan')

    def record(self,witness):
        return dict(classifier_identity=self.identity,**recover(self.baseline.session,witness))


class Aggregate:
    def __init__(self,classifier):
        self.classifier=classifier;self.count=0;self.poisoned=False;self.edges=0;self.model_only=0
        self.kinds=Counter(dict.fromkeys(KINDS,0));self.relations=Counter(dict.fromkeys(RELATIONS,0))
        self.exact=Counter(dict.fromkeys(KINDS,0));self.interior=Counter(dict.fromkeys(KINDS,0));self.free=Counter(dict.fromkeys(KINDS,0))
        self.invalid=Counter(dict.fromkeys(INVALID,0));self.adjacencies=Counter(dict.fromkeys(KINDS,0))
        self.examples_kind={k:[] for k in KINDS};self.examples_relation={k:[] for k in RELATIONS}
        self.work=Counter(dict.fromkeys(c56.WORK,0))

    def add(self,record):
        if self.poisoned:raise ValueError('classifier aggregate already poisoned')
        try:self._add(record)
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):
            self.poisoned=True
            raise

    def _add(self,record):
        c=self.classifier
        if self.count>=len(c.baseline.witnesses):raise ValueError('duplicate/extra classification source row')
        witness=c.baseline.witnesses[self.count]
        if type(record)!=dict or record.get('classifier_identity')!=c.identity:
            raise ValueError('wrong image/tool/schema/contract/classifier identity')
        # Independent bounded replay validates exact FIRST source instruction,
        # all crossing successors, stale bytes, target relations and row order.
        expected=c.record(witness)
        if c56.encoded(record)!=c56.encoded(expected):raise ValueError('missing/duplicate/out-of-order/stale/malformed boundary classification')
        self.work.update(record['recovery_work'])
        for edge in record['edges']:
            kind,relation=edge['edge_kind'],edge['target_metadata_relation']
            self.edges+=1;self.kinds[kind]+=1;self.relations[relation]+=1
            self.model_only+=not edge['architectural_successor']
            if relation=='exact_unique_runtime_begin':self.exact[kind]+=1
            elif relation=='inside_unique_runtime_span_nonentry':self.interior[kind]+=1
            elif relation=='executable_file_backed_outside_runtime_metadata':self.free[kind]+=1
            else:self.invalid[relation]+=1
            if edge['exact_adjacency'] is True:self.adjacencies[kind]+=1
            example={k:v for k,v in record.items() if k not in {'edges','recovery_work','classifier_identity'}}
            example.update(edge)
            if len(self.examples_kind[kind])<c.limit:self.examples_kind[kind].append(example)
            if len(self.examples_relation[relation])<c.limit:self.examples_relation[relation].append(example)
        self.count+=1

    def finish(self):
        c=self.classifier;c.assert_current();baseline=c.baseline.result
        if self.poisoned or self.count!=c.baseline.acceptance.boundary_rows or self.count!=len(c.baseline.witnesses):
            self.poisoned=True
            raise ValueError('poisoned/missing accepted boundary-refusal rows')
        if (sum(self.kinds.values())!=self.edges or sum(self.relations.values())!=self.edges or
            sum(self.exact.values())+sum(self.interior.values())+sum(self.free.values())+sum(self.invalid.values())!=self.edges):
            self.poisoned=True;raise ValueError('classification count mismatch')
        preserved=('runtime_functions_in_metadata','runtime_rows_covered','runtime_functions_not_considered',
            'reference_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates',
            'reference_candidates','external_anchors','anchor_counted_as_local_candidate','publication_source_kind',
            'c52_production_watch_gate','live_qword_reads')
        return dict(repository_result=READY,image_authenticated=True,image_sha256=c.header['image_sha256'],
            tool_version=VERSION,schema_version=SCHEMA,evidence='OFFLINE_STATIC_DIAGNOSTIC_ONLY',
            classifier_header=c.header,classifier_identity=c.identity,
            boundary_refused_rows_expected=c.baseline.acceptance.boundary_rows,
            boundary_refused_rows_classified=self.count,boundary_edges_total=self.edges,
            boundary_edges_scope='existing_c53_successors_model_only_edges_explicitly_labeled',
            architectural_boundary_edges_total=self.edges-self.model_only,model_only_boundary_edges_total=self.model_only,
            edge_kind_counts=dict(self.kinds),target_metadata_relation_counts=dict(self.relations),
            exact_runtime_begin_edge_kind_counts=dict(self.exact),interior_target_edge_kind_counts=dict(self.interior),
            metadata_free_RX_edge_kind_counts=dict(self.free),ambiguous_or_invalid_target_counts=dict(self.invalid),
            exact_adjacency_edge_kind_counts=dict(self.adjacencies),
            examples_by_edge_kind=self.examples_kind,examples_by_target_relation=self.examples_relation,
            diagnostic_validation_replay_work=dict(self.work),
            diagnostic_work_scope='validation_replay_only_generation_replays_same_bounded_graph_no_c56_counter_comparison',
            classification_complete=True,cross_runtime_fact_propagation=False,
            c56_candidate_counts_unchanged=True,**{k:baseline[k] for k in preserved},
            **{k:False for k in c56.FALSE_GATES})


def analyze(data,cache=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:raise ValueError('exact executable SHA256 mismatch or input cap')
    session=c56.Session(data)
    baseline=collect_baseline(session,cache,progress=progress)
    classifier=Classifier(baseline);aggregate=Aggregate(classifier)
    for index,witness in enumerate(baseline.witnesses):
        aggregate.add(classifier.record(witness))
        if progress is not None and ((index+1)%1024==0 or index+1==len(baseline.witnesses)):
            progress('boundary classification',index+1,len(baseline.witnesses))
    return aggregate.finish()


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--c56-checkpoint-dir',type=Path)
    args=parser.parse_args();authenticated=False
    try:
        if args.output.suffix.lower()!='.json' or args.output.resolve()==args.exe.resolve():raise ValueError('invalid diagnostic output path')
        if args.c56_checkpoint_dir is not None and args.output.resolve().is_relative_to(args.c56_checkpoint_dir.resolve()):
            raise ValueError('output must stay outside read-only c56 checkpoints')
        if args.exe.stat().st_size>cfg.old.MAX_FILE:raise ValueError('input file cap')
        with args.exe.open('rb') as f:data=f.read(cfg.old.MAX_FILE+1)
        if c56.sha(data)!=EXPECTED_SHA:raise ValueError('exact executable SHA256 mismatch')
        authenticated=True
        result=analyze(data,args.c56_checkpoint_dir,lambda stage,n,total:print(f'c57 {stage} {n}/{total}',file=sys.stderr))
        c56.atomic_json(args.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(c56.encoded(dict(repository_result=BLOCKED,image_authenticated=authenticated,
            classification_complete=False,cross_runtime_fact_propagation=False,aggregate_published=False,
            failures=[dict(reason=str(error))],previous_output_preserved=True)).decode())
        raise SystemExit(2)
    print(c56.encoded(result).decode())


if __name__=='__main__':main()
