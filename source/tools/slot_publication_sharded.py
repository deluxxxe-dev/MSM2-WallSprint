"""c56 OFFLINE deterministic/resumable runtime-row coverage; candidates only.

The CLI authenticates the pinned frozen image and decoder. Shards reuse c53-c55
without new value semantics. Cache checksums detect stale/corrupt local evidence;
they are not signatures or certificates for untrusted third-party results.
"""
import argparse
from collections import Counter
from dataclasses import asdict, dataclass
import hashlib
import json
import os
from pathlib import Path
import struct
import sys
import tempfile

import slot_publication_direct_entry as c55

cfg=c55.cfg
EXPECTED_SHA=cfg.EXPECTED_SHA
VERSION='v0.1c56'
SCHEMA=1
CONTRACT='deterministic_contiguous_runtime_metadata_shards_v1'
READY='SHARDED_SLOT_PUBLICATION_SCANNER_READY'
BLOCKED='SHARDED_SLOT_PUBLICATION_SCANNER_BLOCKED'
WORK=('instructions_decoded','decoded_bytes','file_code_bytes_loaded','states_discovered',
      'edges_traversed','worklist_iterations','blocks_traversed')
CALLS=('call_edges_with_any_preserved_origin','preserved_origin_register_facts_across_calls')
OPS=('READ','WRITE','RMW','ADDRESS_ONLY')
SUMMARY=('c54_runtime_entry_summary_attempted','c54_runtime_entry_summary_admitted',
    'c54_runtime_entry_summary_refused','c54_runtime_instructions_decoded',
    'c54_runtime_code_bytes_loaded','c54_runtime_work','c54_runtime_cache_hits',
    'direct_entry_summary_attempted','direct_entry_summary_admitted','direct_entry_summary_refused',
    'direct_entry_instructions_decoded','direct_entry_code_bytes_loaded','direct_entry_work',
    'direct_entry_cache_hits','direct_entry_nested_summaries_used','direct_entry_cycles_refused',
    'call_edge_verification_instructions_decoded','summary_requests')
FALSE_GATES=('candidate_sweep_complete','writer_enumeration_complete','allocation_proven',
    'base_proven','range_proven','lifetime_proven','ownership_exclusive','destination_contained',
    'CFG_continuation','alternate_entries_EH_indirect_writers_closed','production_integrated',
    'hardware_watch_enabled','pointer_chase','game_used')
MAX_JSON=8*1024*1024


def encoded(value):return json.dumps(value,sort_keys=True,separators=(',',':'),allow_nan=False).encode()
def digest(value):return hashlib.sha256(encoded(value)).hexdigest()
def sha(raw):return hashlib.sha256(raw).hexdigest()


@dataclass(frozen=True)
class Profile:
    rows_per_shard:int=128
    records_per_shard:int=128
    records_total:int=256
    main:cfg.Caps=cfg.Caps()
    summary:c55.Caps=c55.Caps()

    def check(self):
        if any(type(n)!=int for n in (self.rows_per_shard,self.records_per_shard,self.records_total)) or not 1<=self.rows_per_shard<=128 or not 1<=self.records_per_shard<=256 or not 1<=self.records_total<=256:
            raise ValueError('invalid shard/record bounds')
        if any(type(n)!=int or n<=0 for n in asdict(self.main).values()) or any(type(n)!=int or n<=0 for n in asdict(self.summary).values()):
            raise ValueError('invalid analysis bounds')
        if any(n>asdict(cfg.Caps())[k] for k,n in asdict(self.main).items()) or any(n>asdict(c55.Caps())[k] for k,n in asdict(self.summary).items()):
            raise ValueError('historical analysis caps must not be widened')
        # Each shard's main-work upper bound fits the original global caps.
        for global_name,local_name in [('bytes','region_bytes'),('instructions','region_instructions'),
                ('states','region_states'),('edges','region_edges'),('iterations','region_iterations'),('blocks','region_blocks')]:
            if self.rows_per_shard*getattr(self.main,local_name)>getattr(self.main,global_name):
                raise ValueError('shard upper bound exceeds main work cap')


def source_identity():
    names=('slot_publication.py','slot_publication_cfg.py','slot_publication_callee.py',
           'slot_publication_direct_entry.py','slot_publication_sharded.py')
    return {name:sha((Path(__file__).parent/name).read_bytes()) for name in names}


def intervals(count,size):return [(start,min(count,start+size)) for start in range(0,count,size)]


def summary_counts(engine):
    r=engine.runtime.metrics
    values=dict(engine.metrics)
    values.update(c54_runtime_entry_summary_attempted=r['callee_summaries_attempted'],
        c54_runtime_entry_summary_admitted=r['callee_summaries_admitted'],
        c54_runtime_entry_summary_refused=r['callee_summaries_refused'],
        c54_runtime_instructions_decoded=r['summary_instructions_decoded'],
        c54_runtime_code_bytes_loaded=r['summary_code_bytes_loaded'],c54_runtime_work=r['summary_work'],
        c54_runtime_cache_hits=r['summary_cache_hits'])
    return {k:values.get(k,0) for k in SUMMARY}


class Session:
    def __init__(self,data,profile=Profile()):
        profile.check()
        if len(data)>cfg.old.MAX_FILE:raise ValueError('input file cap')
        self.data=bytes(data)
        if sha(self.data)!=EXPECTED_SHA:raise ValueError('exact executable SHA256 mismatch')
        cfg.old.decoder()  # pinned5.0.6, even for an empty row table
        sections,names,base,size,excluded,read,rows=cfg.image_metadata(self.data)
        if not excluded:raise ValueError('deferred producer exclusion unavailable')
        slot=[s for s in sections if s[0]<=cfg.SLOT and cfg.SLOT+8<=s[1]]
        if len(slot)!=1 or not slot[0][4]&0x40000000 or slot[0][4]&0x20000000:
            raise ValueError('slot must be unique readable non-executable image storage')
        self.profile,self.sections,self.names,self.base,self.size=profile,tuple(sections),names,base,size
        self.excluded,self.read,self.rows=tuple(excluded),read,tuple(rows)
        self.plan=intervals(len(rows),profile.rows_per_shard)
        self.domain=c55.Domain(image_id=EXPECTED_SHA,rows=self.rows,sections=self.sections,
            image_size=size,excluded=self.excluded,read=read)
        self.header=dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_authenticated=True,image_sha256=EXPECTED_SHA,decoder_version=cfg.cs.__version__,
            runtime_functions_in_metadata=len(rows),runtime_metadata_sha256=sha(b''.join(struct.pack('<III',*r) for r in rows)),
            profile=asdict(profile),tool_sources=source_identity(),
            contracts=dict(c53='single_runtime_entry_normal_CFG_must_v1',c54=c55.c54.CONTRACT,c55=c55.CONTRACT),
            summary_scope='fresh_c55_engine_per_shard_no_cross_shard_cache_or_partition_union',
            row_policy='whole_row_refused_no_candidates_on_any_observed_summary_cap')
        self.manifest=dict(header=self.header,manifest_id=digest(self.header))
        self.anchor_attempted=False
        self.anchor,self.anchor_status=self.external_anchor()

    def assert_current_tool(self):
        if source_identity()!=self.header['tool_sources']:raise ValueError('tool sources changed during scan')

    def row_identity(self,index):
        return sha(struct.pack('<IIII',index,*self.rows[index]))

    def preflight(self,index):
        begin,end,_=self.rows[index]
        if any(begin<e and b<end for b,e in self.excluded):return 'deferred_region_excluded',None,None
        sections=[s for s in self.sections if s[0]<=begin and end<=s[1]
            and s[4]&0x60000000==0x60000000 and not s[4]&0x80000000]
        if len(sections)!=1:return 'unsupported_runtime_section',None,None
        if end-begin>self.profile.main.region_bytes:return 'region_byte_cap',None,None
        try:code=self.read(begin,end-begin)
        except ValueError:return 'runtime_span_not_file_backed',None,None
        return None,code,sections[0]

    def external_anchor(self):
        pc=cfg.old.ANCHOR_SITE
        if any(pc<e and b<pc+7 for b,e in self.excluded):return [],'excluded_not_decoded'
        sections=[s for s in self.sections if s[0]<=pc and pc+7<=min(s[1],s[0]+s[3])
            and s[4]&0x60000000==0x60000000 and not s[4]&0x80000000]
        if len(sections)!=1:return [],'unavailable_or_unmatched'
        self.anchor_attempted=True
        raw=self.read(pc,7);anchor=cfg.old.external_anchor_record(raw,self.base,self.size)
        if not anchor:return [],'unavailable_or_unmatched'
        anchor.update(instruction_sha256=sha(raw),operation='READ',
            scope='authenticated_external_c28_not_local_CFG_not_duplicated_in_shards')
        return [anchor],'authenticated_external_c28_not_local_CFG'

    def scan_row(self,index,engine):
        begin,end,unwind=self.rows[index]
        reason,code,section=self.preflight(index)
        row=dict(index=index,metadata_row_sha256=self.row_identity(index),status='REFUSED',reason=reason,
            body_code_sha256=sha(code) if code is not None else None,
            work={k:0 for k in WORK},calls={k:0 for k in CALLS},unresolved_memory_reasons={},
            operation_counts={k:0 for k in OPS})
        if reason:
            if reason=='deferred_region_excluded':row['status']='EXCLUDED'
            return row,[]
        summary_capped=False
        def summary_call(target,edge):
            nonlocal summary_capped
            proof=engine.for_site(target,edge)
            if proof['status']=='REFUSED' and any(term in (proof.get('refusal_reason') or '') for term in ('cap','budget','exhausted')):
                summary_capped=True  # includes cached negative cap results
            return proof
        result=cfg.scan_region(code,begin,image_base=self.base,image_size=self.size,
            caps=self.profile.main,excluded=self.excluded,
            call_target_allowed=lambda pc:any(b<=pc<min(e,b+n) and flags&0x60000000==0x60000000 and not flags&0x80000000
                for b,e,_,n,flags in self.sections),call_site_preservation=summary_call)
        row['work']={k:result['stats'].get(k,0) for k in WORK}
        row['unresolved_memory_reasons']=result['unresolved_reasons']
        if result['status']!='MODELED_NORMAL_CFG':
            row['reason']=next(iter(result['stopped_paths']),'refused_region');return row,[]
        if summary_capped:
            row['reason']='summary_cap_refused';return row,[]
        row.update(status='MODELED',reason=None,calls={k:result['stats'].get(k,0) for k in CALLS})
        refs=result['references']
        for ref in refs:
            ref['section']=dict(name=self.names[section[0]],begin_rva=section[0],end_rva=section[1])
            ref['runtime_function']['unwind_rva']=unwind
            row['operation_counts'][ref['operation']]+=1
        return row,refs

    def make_shard(self,shard_id):
        self.assert_current_tool()
        start,end=self.plan[shard_id];engine=c55.Engine(self.domain,self.profile.summary)
        rows,records=[],[]
        for index in range(start,end):
            row,refs=self.scan_row(index,engine);rows.append(row)
            for ref in refs[:max(0,self.profile.records_per_shard-len(records))]:
                records.append(dict(ref,source_shard=shard_id,runtime_row_index=index,
                    runtime_row_sha256=self.row_identity(index)))
        report=engine.report()
        payload=dict(manifest_id=self.manifest['manifest_id'],shard_id=shard_id,interval=[start,end],
            rows=rows,candidate_records=records,summary_counts=summary_counts(engine),
            summary_evidence_sha256=digest(report),summary_budget_exhausted=engine.closed,
            summary_diagnostics_complete=report['diagnostic_counts_complete'])
        self.assert_current_tool()
        return dict(payload=payload,payload_sha256=digest(payload))


def natural(n,limit=2**63-1):
    if type(n)!=int or not 0<=n<=limit:raise ValueError('malformed nonnegative bounded counter')


def counters(table,keys,limit):
    if type(table)!=dict or set(table)!=set(keys):raise ValueError('malformed counter schema')
    for n in table.values():natural(n,limit)


class Aggregate:
    def __init__(self,session):
        self.session=session;self.count=0;self.covered=0
        self.work=Counter(dict.fromkeys(WORK,0));self.calls=Counter(dict.fromkeys(CALLS,0))
        self.summary=Counter(dict.fromkeys(SUMMARY,0));self.operations=Counter(dict.fromkeys(OPS,0))
        self.statuses,self.reasons,self.unresolved=Counter(),Counter(),Counter()
        self.records,self.intervals,self.digests=[],[],[]
        self.summary_exhausted=0;self.summary_diagnostics_incomplete=0
        self.validation_body_bytes=0;self.validation_candidate_bytes=0;self.validation_candidate_decodes=0
        self.poisoned=False

    def add(self,envelope):
        if self.poisoned:raise ValueError('aggregate already poisoned')
        try:self._add(envelope)
        except (ValueError,KeyError,TypeError,IndexError):
            self.poisoned=True
            raise

    def _add(self,envelope):
        s=self.session
        if type(envelope)!=dict or set(envelope)!={'payload','payload_sha256'}:raise ValueError('malformed shard envelope')
        p=envelope['payload']
        required={'manifest_id','shard_id','interval','rows','candidate_records','summary_counts',
                  'summary_evidence_sha256','summary_budget_exhausted','summary_diagnostics_complete'}
        if type(p)!=dict or set(p)!=required or digest(p)!=envelope['payload_sha256']:raise ValueError('malformed/corrupt shard payload')
        if p['manifest_id']!=s.manifest['manifest_id']:raise ValueError('wrong image/tool/schema/contracts/profile/metadata/source identity')
        natural(p['shard_id'],len(s.plan))
        if type(p['interval'])!=list or len(p['interval'])!=2 or any(type(n)!=int for n in p['interval']):
            raise ValueError('malformed interval')
        if self.count>=len(s.plan) or p['shard_id']!=self.count or p['interval']!=list(s.plan[self.count]):
            raise ValueError('missing/duplicate/overlapping/out-of-order/out-of-range interval')
        start,stop=s.plan[self.count]
        if start!=self.covered or type(p['rows'])!=list or len(p['rows'])!=stop-start:raise ValueError('incomplete row interval')
        if type(p['summary_budget_exhausted'])!=bool or type(p['summary_diagnostics_complete'])!=bool:
            raise ValueError('malformed summary diagnostic flags')
        hex_hash(p['summary_evidence_sha256'])
        limit=(s.profile.main.region_instructions*(stop-start)+2*s.profile.summary.work)*32
        counters(p['summary_counts'],SUMMARY,limit)
        for prefix in ['c54_runtime_entry_summary','direct_entry_summary']:
            if p['summary_counts'][prefix+'_attempted']!=p['summary_counts'][prefix+'_admitted']+p['summary_counts'][prefix+'_refused']:
                raise ValueError('summary attempt partition mismatch')
        sc=p['summary_counts']
        if sc['c54_runtime_entry_summary_attempted']+sc['direct_entry_summary_attempted']+sc['c54_runtime_cache_hits']+sc['direct_entry_cache_hits']>sc['summary_requests']:
            raise ValueError('summary requests/cache/attempt counts disagree')
        for count,work in [('c54_runtime_instructions_decoded','c54_runtime_work'),
                ('c54_runtime_code_bytes_loaded','c54_runtime_work'),('direct_entry_instructions_decoded','direct_entry_work'),
                ('direct_entry_code_bytes_loaded','direct_entry_work'),('call_edge_verification_instructions_decoded','direct_entry_work')]:
            if sc[count]>sc[work]:raise ValueError('summary work accounting mismatch')
        for kind,key,cap in [('direct','direct_entry_work',s.profile.summary.work),
                             ('runtime','c54_runtime_work',s.profile.summary.work),
                             ('direct bytes','direct_entry_code_bytes_loaded',s.profile.summary.bytes),
                             ('runtime bytes','c54_runtime_code_bytes_loaded',s.profile.summary.bytes)]:
            natural(p['summary_counts'][key],cap)
        row_keys={'index','metadata_row_sha256','status','reason','body_code_sha256','work','calls','unresolved_memory_reasons','operation_counts'}
        row_map={};row_sections={}
        for offset,row in enumerate(p['rows']):
            index=start+offset
            if type(row)!=dict or set(row)!=row_keys or type(row['index'])!=int or row['index']!=index or row['metadata_row_sha256']!=s.row_identity(index):
                raise ValueError('wrong/missing/duplicate/stale runtime row identity')
            reason,code,section=s.preflight(index)
            row_sections[index]=section
            self.validation_body_bytes+=len(code) if code is not None else 0
            if row['body_code_sha256']!=(sha(code) if code is not None else None):raise ValueError('stale runtime code evidence')
            if row['status'] not in {'MODELED','REFUSED','EXCLUDED'}:raise ValueError('unknown row outcome')
            if reason:
                if row['reason']!=reason or row['status']!=('EXCLUDED' if reason=='deferred_region_excluded' else 'REFUSED'):
                    raise ValueError('wrong deferred/section/boundary refusal')
            elif row['status']=='EXCLUDED':raise ValueError('unjustified excluded row')
            if row['status']=='MODELED' and row['reason'] is not None:raise ValueError('modeled row has refusal')
            if row['status']!='MODELED' and (type(row['reason'])!=str or not 1<=len(row['reason'])<=256):
                raise ValueError('missing refusal reason')
            counters(row['work'],WORK,s.profile.main.region_bytes*32)
            work_limits=dict(instructions_decoded=s.profile.main.region_instructions,
                decoded_bytes=s.profile.main.region_bytes,file_code_bytes_loaded=s.profile.main.region_bytes,
                states_discovered=s.profile.main.region_states,edges_traversed=s.profile.main.region_edges,
                worklist_iterations=s.profile.main.region_iterations,blocks_traversed=s.profile.main.region_blocks)
            for key,cap in work_limits.items():natural(row['work'][key],cap)
            if row['work']['file_code_bytes_loaded']!=(len(code) if code is not None else 0):
                raise ValueError('row code-byte accounting mismatch')
            if row['status']=='MODELED' and (row['work']['instructions_decoded']==0 or row['work']['states_discovered']==0
                    or row['work']['blocks_traversed']==0 or row['work']['decoded_bytes']!=len(code)):
                raise ValueError('modeled row lacks complete decoding/work')
            counters(row['calls'],CALLS,s.profile.main.region_instructions*16)
            counters(row['operation_counts'],OPS,s.profile.main.references)
            if sum(row['operation_counts'].values())>s.profile.main.references:raise ValueError('region reference cap exceeded')
            if type(row['unresolved_memory_reasons'])!=dict or len(row['unresolved_memory_reasons'])>128:
                raise ValueError('malformed unresolved reasons')
            for key,n in row['unresolved_memory_reasons'].items():
                if type(key)!=str or not 1<=len(key)<=160:raise ValueError('malformed unresolved reason')
                natural(n,s.profile.main.region_instructions*16)
            if row['status']!='MODELED' and (any(row['operation_counts'].values()) or any(row['calls'].values())):
                raise ValueError('candidate/preserved CALL escapes refused region')
            if reason and (any(row['work'].values()) or row['unresolved_memory_reasons']):raise ValueError('work claimed before domain admission')
            self.work.update(row['work']);self.calls.update(row['calls']);self.operations.update(row['operation_counts'])
            self.statuses[row['status']]+=1
            if row['reason']:self.reasons[row['reason']]+=1
            self.unresolved.update(row['unresolved_memory_reasons']);row_map[index]=row
        records=p['candidate_records']
        if type(records)!=list or len(records)>s.profile.records_per_shard:raise ValueError('candidate record cap exceeded')
        expected=min(s.profile.records_per_shard,sum(sum(r['operation_counts'].values()) for r in p['rows']))
        if len(records)!=expected:raise ValueError('missing candidate records without declared bound')
        seen=set();per_row=Counter();last_order=None
        candidate_keys={'site_rva','instruction_sha256','mnemonic','operands','operand_index','operand_width',
            'access_width','operation','may_write','decoded_access','access_proof','effective_rva','overlap',
            'address_proof_class','address_proof_chain','cfg','runtime_function','section',
            'source_shard','runtime_row_index','runtime_row_sha256'}
        for ref in records:
            if type(ref)!=dict or set(ref)!=candidate_keys:raise ValueError('malformed candidate schema')
            if type(ref['cfg'])!=dict or type(ref['section'])!=dict or type(ref['overlap'])!=dict:
                raise ValueError('malformed candidate nested metadata')
            index=ref.get('runtime_row_index');natural(index,len(s.rows)-1)
            natural(ref['source_shard'],len(s.plan))
            if index not in row_map or row_map[index]['status']!='MODELED' or ref.get('source_shard')!=self.count or ref.get('runtime_row_sha256')!=s.row_identity(index):
                raise ValueError('candidate has wrong shard/row identity or refused source')
            begin,end,unwind=s.rows[index]
            if ref.get('runtime_function')!=dict(begin_rva=begin,end_rva=end,unwind_rva=unwind):raise ValueError('candidate runtime provenance mismatch')
            section=row_sections[index]
            if ref['section']!=dict(name=s.names[section[0]],begin_rva=section[0],end_rva=section[1]):
                raise ValueError('candidate section provenance mismatch')
            pc=ref.get('site_rva');natural(pc,s.size-1)
            if not begin<=pc<end:raise ValueError('candidate site outside row')
            raw=s.read(pc,min(15,end-pc));ins=next(cfg.old.decoder().disasm(raw,pc,count=1),None)
            self.validation_candidate_bytes+=len(raw);self.validation_candidate_decodes+=1
            if ins is None or sha(bytes(ins.bytes))!=ref.get('instruction_sha256'):raise ValueError('candidate stale instruction')
            op=ref.get('operand_index');natural(op,len(ins.operands)-1)
            if ins.operands[op].type!=cfg.x.X86_OP_MEM or cfg.access_kind(ins,op)[0]!=ref.get('operation'):
                raise ValueError('candidate access semantics mismatch')
            if ref['operand_width']!=ins.operands[op].size or ref['decoded_access']!=ins.operands[op].access or ref['access_proof']!=cfg.access_kind(ins,op)[1]:
                raise ValueError('candidate operand metadata mismatch')
            width=0 if ref['operation']=='ADDRESS_ONLY' else ins.operands[op].size
            natural(ref['access_width'],64)
            if type(ref['may_write'])!=bool or ref['mnemonic']!=ins.mnemonic or ref['operands']!=ins.op_str:
                raise ValueError('candidate instruction metadata mismatch')
            if ref.get('access_width')!=width or ref.get('may_write')!=(ref['operation'] in {'WRITE','RMW'}):raise ValueError('candidate width/write mismatch')
            effective=ref.get('effective_rva');natural(effective,s.size-1)
            low,high=max(effective,cfg.SLOT),min(effective+max(1,width),cfg.SLOT+8)
            if low>=high or ref.get('overlap')!=dict(begin_rva=low,end_rva=high,bytes=high-low,address_point_only=width==0):raise ValueError('candidate exact slot overlap mismatch')
            if ref.get('cfg',{}).get('entry_rva')!=begin or ref['cfg'].get('fixpoint_complete') is not True:raise ValueError('candidate lacks completed CFG')
            if ref['cfg'].get('address_fact_agrees_on_all_reachable_incoming_states') is not True:
                raise ValueError('candidate lacks must-analysis proof')
            cfg_keys={'entry_rva','block_rva','incoming_edges','incoming_predecessor_rvas','graph_sha256',
                'region_reachable_nodes','region_joins','region_loop_back_edges','fixpoint_complete',
                'address_fact_agrees_on_all_reachable_incoming_states','scope','call_preservation_graph_sha256',
                'callsite_summary_count','preservation_scope'}
            graph=ref['cfg']
            if set(graph)!=cfg_keys or graph['scope']!='conditional_normal_CFG_from_single_runtime_entry_not_all_instruction_entries' or graph['preservation_scope']!='per_register_never_written_no_ABI_or_return_value_origin':
                raise ValueError('candidate CFG contract/schema mismatch')
            natural(graph['block_rva'],end-1)
            if graph['block_rva']<begin or graph['block_rva']>pc:raise ValueError('candidate block outside row')
            for key in ('incoming_edges','region_reachable_nodes','region_joins','region_loop_back_edges','callsite_summary_count'):
                natural(graph[key],s.profile.main.region_edges)
            predecessors=graph['incoming_predecessor_rvas']
            if type(predecessors)!=list or len(predecessors)!=graph['incoming_edges']:raise ValueError('candidate predecessor mismatch')
            for predecessor in predecessors:
                natural(predecessor,end-1)
                if predecessor<begin:raise ValueError('candidate predecessor outside row')
            if predecessors!=sorted(set(predecessors)) or graph['region_reachable_nodes']!=row_map[index]['work']['states_discovered']:
                raise ValueError('candidate CFG node accounting mismatch')
            chain=ref['address_proof_chain']
            if type(chain)!=list or len(chain)>s.profile.main.hops or ref['address_proof_class'] not in {'cfg_must_exact_static_image_address','direct_rip','direct_absolute','register_affine_static'}:
                raise ValueError('candidate address proof schema mismatch')
            for step in chain:
                if type(step)!=list or len(step)!=3 or step[1] not in {'rip_lea','affine_lea','copy64'}:
                    raise ValueError('candidate address proof step mismatch')
                natural(step[0],end-1);natural(step[2],s.size-1)
                if step[0]<begin:raise ValueError('candidate origin step outside caller row')
            hex_hash(ref['cfg'].get('graph_sha256'))
            hex_hash(ref['cfg'].get('call_preservation_graph_sha256'))
            order=(index,pc,op)
            if order in seen or last_order is not None and order<=last_order:raise ValueError('duplicate/out-of-order candidate')
            seen.add(order);last_order=order;per_row[index,ref['operation']]+=1
            if per_row[index,ref['operation']]>row_map[index]['operation_counts'][ref['operation']]:raise ValueError('candidate exceeds row count')
            if len(self.records)<s.profile.records_total:self.records.append(ref)
        remaining=s.profile.records_per_shard
        for index,row in row_map.items():
            expected=min(remaining,sum(row['operation_counts'].values()))
            if sum(per_row[index,op] for op in OPS)!=expected:raise ValueError('candidate records are not deterministic row prefix')
            remaining-=expected
        self.summary.update(p['summary_counts']);self.summary_exhausted+=p['summary_budget_exhausted']
        self.summary_diagnostics_incomplete+=not p['summary_diagnostics_complete']
        self.intervals.append([start,stop]);self.digests.append(envelope['payload_sha256'])
        self.covered=stop;self.count+=1

    def finish(self):
        s=self.session;s.assert_current_tool()
        if self.poisoned or self.covered!=len(s.rows) or self.count!=len(s.plan):
            self.poisoned=True
            raise ValueError('poisoned/missing shards; incomplete runtime table coverage')
        total=sum(self.operations.values())
        return dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,evidence='OFFLINE_STATIC',
            image_authenticated=True,image_sha256=s.header['image_sha256'],manifest_id=s.manifest['manifest_id'],
            runtime_functions_in_metadata=len(s.rows),runtime_rows_covered=self.covered,runtime_functions_not_considered=0,
            shard_count=self.count,shard_intervals=self.intervals,shard_payload_hashes=self.digests,shard_failures=[],
            main_coverage=dict(self.work,regions_considered=self.covered,regions_modeled=self.statuses['MODELED'],
                regions_refused=self.statuses['REFUSED'],regions_excluded=self.statuses['EXCLUDED']),
            row_refusal_reasons=dict(sorted(self.reasons.items())),**dict(self.summary),**dict(self.calls),
            unresolved_memory_reasons=dict(sorted(self.unresolved.items())),
            call_clobber_all_address_origins_remaining=self.unresolved['call_clobber_all_address_origins'],
            c53_call_clobber_all_address_origins_baseline=324714,
            reference_candidates=self.records,reference_candidate_count=total,reader_candidates=self.operations['READ'],
            writer_candidates=self.operations['WRITE']+self.operations['RMW'],rmw_candidates=self.operations['RMW'],
            address_only_candidates=self.operations['ADDRESS_ONLY'],candidate_records_retained=len(self.records),
            candidate_records_not_retained=total-len(self.records),candidate_record_limit=s.profile.records_total,
            external_anchors=s.anchor,external_anchor_status=s.anchor_status,anchor_counted_as_local_candidate=False,
            anchor_decode_attempts_once=int(s.anchor_attempted),
            anchor_instructions_decoded_once=(1 if s.anchor else None if s.anchor_attempted else 0),
            anchor_file_code_bytes_loaded_once=7*int(s.anchor_attempted),
            aggregate_validation_body_hash_bytes=self.validation_body_bytes,
            aggregate_validation_candidate_decode_attempts=self.validation_candidate_decodes,
            aggregate_validation_candidate_code_bytes=self.validation_candidate_bytes,
            shards_summary_budget_exhausted=self.summary_exhausted,shards_summary_diagnostics_incomplete=self.summary_diagnostics_incomplete,
            count_scope='additive_row_work_and_shard_local_proof_attempts_not_global_unique_callees_or_single_c55_comparison',
            total_main_and_summary_instructions_decoded=self.work['instructions_decoded']+
                self.summary['c54_runtime_instructions_decoded']+self.summary['direct_entry_instructions_decoded']+
                self.summary['call_edge_verification_instructions_decoded'],
            total_file_code_bytes_loaded_including_summaries_and_anchor=self.work['file_code_bytes_loaded']+
                self.summary['c54_runtime_code_bytes_loaded']+self.summary['direct_entry_code_bytes_loaded']+7*int(s.anchor_attempted),
            excluded_deferred_regions=s.excluded,publication_source_kind='unproven',live_qword_reads=0,
            c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
            remaining_domains=['executable_gaps','deferred_producer','refused_regions','unsupported_semantics',
                'alternate_entries_EH','indirect_targets_and_writers','publication_allocation_ownership_lifetime'],
            **{key:False for key in FALSE_GATES})


def hex_hash(value):
    if type(value)!=str or len(value)!=64 or any(c not in '0123456789abcdef' for c in value):raise ValueError('malformed SHA256')


def read_json(path):
    if path.is_symlink() or not path.is_file() or path.stat().st_size>MAX_JSON:raise ValueError('invalid/capped checkpoint file')
    def unique(pairs):
        result={}
        for key,value in pairs:
            if key in result:raise ValueError('duplicate JSON key')
            result[key]=value
        return result
    def invalid(value):raise ValueError('nonfinite JSON value')
    return json.loads(path.read_text(encoding='utf-8'),object_pairs_hook=unique,parse_constant=invalid)


def atomic_json(path,value):
    if path.is_symlink():raise ValueError('refuse symlink output')
    raw=encoded(value)+b'\n'
    if len(raw)>MAX_JSON:raise ValueError('compact JSON file cap')
    temp=None
    try:
        with tempfile.NamedTemporaryFile(dir=path.parent,prefix='.c56-',suffix='.tmp',delete=False) as out:
            temp=Path(out.name);out.write(raw);out.flush();os.fsync(out.fileno())
        os.replace(temp,path)
    finally:
        if temp is not None and temp.exists():temp.unlink()


def run(session,cache,progress=None):
    cache=Path(cache)
    if cache.is_symlink():raise ValueError('refuse symlink checkpoint directory')
    cache.mkdir(parents=True,exist_ok=True)
    manifest=cache/'manifest.json'
    expected={f'shard-{b:06d}-{e:06d}.json' for b,e in session.plan}|{'manifest.json'}
    for path in cache.iterdir():
        if path.name not in expected and not (path.name.startswith('.c56-') and path.suffix=='.tmp' and path.is_file() and not path.is_symlink()):
            raise ValueError('unexpected/stale checkpoint path')
    if manifest.exists():
        if read_json(manifest)!=session.manifest:raise ValueError('stale/wrong image/tool/schema/contracts checkpoint manifest')
    else:
        if any(cache.glob('shard-*.json')):raise ValueError('shards without authenticated manifest')
        atomic_json(manifest,session.manifest)
    aggregate=Aggregate(session)
    for shard_id,(start,end) in enumerate(session.plan):
        path=cache/f'shard-{start:06d}-{end:06d}.json'
        if path.exists():evidence=read_json(path)
        else:
            evidence=session.make_shard(shard_id)
            atomic_json(path,evidence)
        aggregate.add(evidence)
        if progress is not None and (aggregate.count%32==0 or aggregate.count==len(session.plan)):
            progress(aggregate.count,len(session.plan),aggregate.covered,len(session.rows))
    return aggregate.finish()


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--checkpoint-dir',type=Path)
    args=parser.parse_args();authenticated=False
    try:
        if args.output.suffix.lower()!='.json':raise ValueError('aggregate output must use .json')
        if args.output.resolve()==args.exe.resolve():raise ValueError('output must not overwrite input image')
        cache=args.checkpoint_dir or args.output.with_suffix('.shards')
        if args.output.resolve().is_relative_to(cache.resolve()):raise ValueError('aggregate output must stay outside checkpoint directory')
        if args.exe.stat().st_size>cfg.old.MAX_FILE:raise ValueError('input file cap')
        with args.exe.open('rb') as source:session=Session(source.read(cfg.old.MAX_FILE+1))
        authenticated=True
        result=run(session,cache,lambda n,total,rows,count:print(f'c56 verified shards {n}/{total}; runtime rows {rows}/{count}',file=sys.stderr))
        atomic_json(args.output,result)
    except (OSError,ValueError,struct.error,KeyError,TypeError,RecursionError) as error:
        print(encoded(dict(repository_result=BLOCKED,image_authenticated=authenticated,
            image_sha256=EXPECTED_SHA if authenticated else None,
            shard_failures=[dict(reason=str(error))],aggregate_published=False,previous_output_preserved=True)).decode())
        raise SystemExit(2)
    print(encoded(result).decode())


if __name__=='__main__':main()
