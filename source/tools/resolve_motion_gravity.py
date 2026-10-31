"""Fixed-image, bounded relocation plus semantic/CALL checks; no native permission."""
import argparse
from bisect import bisect_right
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import capstone as cs
from capstone import x86 as x
import verify_motion_gravity as verify

pe=verify.pe
MAX_SCAN=192*1024*1024
MAX_SEEDS=100000
MAX_MATCHES=128
MAX_FUNC_BYTES=65536
MAX_FUNCTIONS=16
MAX_DECODE=30000
MAX_OUTPUT=65536
PROFILE_SHA='158f45f34cf25590cd18d586ff4b3e3194bf2a2653b47fa942e6d3d43d510c6d'


def consensus(votes):
    if any(not -(2**63)<=v<2**63 for v in votes.values()):raise pe.Refusal('signed_shift_overflow')
    if len(votes)<3:return None
    counts=Counter(votes.values());best=max(counts.values())
    winners=[v for v,n in counts.items() if n==best]
    return winners[0] if best>=3 and len(winners)==1 else None


class Image:
    def __init__(self,data):
        self.data=data;_,self.image_size,self.sections=pe.pe_sections(data)
        op=struct.unpack_from('<I',data,60)[0]+24
        if struct.unpack_from('<I',data,op+108)[0]<4:raise pe.Refusal('runtime_directory_missing')
        table,size=struct.unpack_from('<II',data,op+112+24)
        if size%12 or size>8*1024*1024 or size//12>600000:raise pe.Refusal('runtime_table_cap')
        self.rows=list(struct.iter_unpack('<III',self.read(table,size))) if size else []
        end=0
        for b,e,u in self.rows:
            if not 0<b<e<=self.image_size or b<end or not 0<u<self.image_size or u%4:raise pe.Refusal('invalid_runtime_ranges')
            end=e
        self.starts=[r[0] for r in self.rows];self.budget=MAX_DECODE;self.cache={}
        self.md=cs.Cs(cs.CS_ARCH_X86,cs.CS_MODE_64);self.md.detail=True
    def read(self,rva,n,code=False):
        found=[s for s in self.sections if s['rva']<=rva and rva+n<=s['rva']+s['size']]
        if len(found)!=1:raise pe.Refusal('unbacked_range')
        s=found[0]
        if code and (s['flags']&0x60000000!=0x60000000 or s['flags']&0x80000000):raise pe.Refusal('non_readonly_executable')
        off=s['raw']+rva-s['rva'];return self.data[off:off+n]
    def covering(self,site):
        j=bisect_right(self.starts,site)-1
        return self.rows[j] if j>=0 and site<self.rows[j][1] else None
    def graph(self,start,end):
        if (start,end) in self.cache:return self.cache[start,end]
        if len(self.cache)>=MAX_FUNCTIONS or not 0<end-start<=MAX_FUNC_BYTES:raise pe.Refusal('function_range_or_count_cap')
        self.read(start,end-start,True)
        todo=[start];seen={};covered={};uncertain=0
        while todo:
            site=todo.pop()
            if site in seen:continue
            if not start<=site<end:uncertain+=1;continue
            if self.budget<=0:raise pe.Refusal('aggregate_decoder_cap')
            self.budget-=1
            i=next(self.md.disasm(self.read(site,min(15,end-site),True),site,count=1),None)
            if not i:uncertain+=1;continue
            for at in range(site,site+i.size):
                if at in covered and covered[at]!=site:raise pe.Refusal('overlapping_CFG_decodes')
                covered[at]=site
            seen[site]=i
            if i.group(cs.CS_GRP_RET) or i.mnemonic in ('ud2','int3','syscall','sysret'):continue
            if i.group(cs.CS_GRP_JUMP):
                if i.operands and i.operands[0].type==x.X86_OP_IMM:todo.append(i.operands[0].imm)
                else:uncertain+=1
                if i.mnemonic=='jmp':continue
            elif i.group(cs.CS_GRP_INT) or i.group(cs.CS_GRP_IRET):continue
            # CALL fallthrough is only a possible static path, never actual-return proof.
            todo.append(site+i.size)
        result=(seen,uncertain);self.cache[start,end]=result;return result
    def boundary(self,start,site,role):
        row=self.covering(start)
        if row and row[0]!=start:return dict(proven=False,reason='primary_inside_existing_function_no_snap')
        if row:end=row[1];kind='exact_PE_entry_CFG'
        elif role=='SetGravityVector':
            end=start+0x4f;kind='reference_RET_bounded_leaf_candidate'
            if any(start<b<end for b in self.starts):return dict(proven=False,reason='leaf_extent_crosses_runtime_entry')
        else:return dict(proven=False,reason='missing_exact_runtime_entry')
        seen,uncertain=self.graph(start,end)
        leaf_ok=row is not None or (start+0x4e in seen and seen[start+0x4e].group(cs.CS_GRP_RET) and not any(i.group(cs.CS_GRP_CALL) for i in seen.values()))
        return dict(proven=site in seen and leaf_ok,kind=kind,runtime=list(row) if row else None,
                    reachable_instruction_count=len(seen),unresolved_edges=uncertain,
                    function_sha256=hashlib.sha256(self.read(start,end-start,True)).hexdigest())


def scan(image,patterns):
    if len(patterns)>8:raise pe.Refusal('pattern_count_cap')
    hits={(p['role'],p['kind']):[] for p in patterns};seeds=0;matches=0
    sections=[s for s in image.sections if s['flags']&0x20000000]
    if sum(s['size'] for s in sections)>MAX_SCAN:raise pe.Refusal('executable_scan_cap')
    for s in sections:
        blob=image.read(s['rva'],s['size'],True)
        for p in patterns:
            if not 1<=p['length']<=128 or len(p['wildcards'])>64:raise pe.Refusal('invalid_pattern_descriptor')
            seed=bytes.fromhex(p['seed_hex'])
            if len(seed)!=8 or not 0<=p['seed_offset']<=p['length']-8 or any(not 0<=w<p['length'] or p['seed_offset']<=w<p['seed_offset']+8 for w in p['wildcards']):raise pe.Refusal('invalid_pattern_seed_mask')
            at=0
            while True:
                hit=blob.find(seed,at)
                if hit<0:break
                at=hit+1;seeds+=1
                if seeds>MAX_SEEDS:raise pe.Refusal('seed_work_cap')
                start=hit-p['seed_offset']
                if start<0 or start+p['length']>len(blob):continue
                candidate=bytearray(blob[start:start+p['length']])
                for w in p['wildcards']:candidate[w]=0
                if hashlib.sha256(candidate).hexdigest()!=p['masked_sha256']:continue
                matches+=1
                if matches>MAX_MATCHES:raise pe.Refusal('pattern_match_cap')
                hits[p['role'],p['kind']].append(s['rva']+start)
    return hits,seeds


def resolve(data,*,expected_sha=pe.EXPECTED_SHA,patterns=None,canonical=verify.TARGETS):
    if len(data)>pe.MAX_FILE:raise pe.Refusal('file_cap')
    if hashlib.sha256(data).hexdigest()!=expected_sha:raise pe.Refusal('unsupported_build_sha256')
    if cs.__version__!='5.0.6':raise pe.Refusal('requires_Capstone5.0.6')
    if patterns is None:
        raw=Path(__file__).with_name('motion_gravity_patterns.json').read_bytes()
        if hashlib.sha256(raw).hexdigest()!=PROFILE_SHA:raise pe.Refusal('unsupported_pattern_profile')
        profile=json.loads(raw)
        if profile['reference_sha256']!='7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662':raise pe.Refusal('unsupported_reference_identity')
        patterns=profile['patterns']
    image=Image(data);hits,seeds=scan(image,patterns)
    roles={};targets={};votes={};ambiguous=False
    for role in canonical:
        primary=hits.get((role,'entry'),[]);interior=hits.get((role,'interior'),[])
        row=dict(primary_matches=primary,interior_matches=interior,status='missing')
        if primary:
            candidates=primary;kind='entry_pattern'
        else:
            mapped=[]
            for site in interior:
                entry=image.covering(site)
                if entry and image.boundary(entry[0],site,role)['proven']:mapped.append(entry[0])
            candidates=sorted(set(mapped));kind='interior_exact_range_CFG'
        if len(candidates)>1:row['status']='ambiguous';ambiguous=True
        elif len(candidates)==1:
            target=candidates[0];proof=image.boundary(target,target,role)
            row.update(candidate_rva=target,boundary=proof,source=kind)
            if proof['proven']:
                row['status']='pattern_resolved';targets[role]=target
                row['delta_from_reference']=target-canonical[role]
                if proof.get('runtime'):votes[role]=target-canonical[role]
            else:row['status']='boundary_unproven'
        roles[role]=row
    shift=consensus(votes)
    # Ambiguity is never repaired with a vote or another role's derived address.
    if not ambiguous and shift is not None:
        for role in canonical:
            row=roles[role]
            if row['status']!='missing':continue
            target=canonical[role]+shift
            if not 0<target<image.image_size:row['status']='fallback_range_rejected';continue
            proof=image.boundary(target,target,role)
            row.update(candidate_rva=target,boundary=proof,source='three_distinct_role_consensus')
            if proof['proven']:row['status']='fallback_candidate';targets[role]=target
            else:row['status']='fallback_boundary_unproven'
    result=dict(schema=1,result='UNRESOLVED_STOP_REFERENCE_RELOCATION_ROUTE',exe_sha256=hashlib.sha256(data).hexdigest(),
                roles=roles,independent_votes=votes,consensus_shift=shift,prefilter_hits=seeds,
                ambiguous=ambiguous,native_ABI_proven=False,lifetime_proven=False,ground_contact_proven=False,
                production_permission=False,live_reads=0,code_exported=False)
    if not ambiguous and len(targets)==4:
        local=verify.verify(data,expected_sha=expected_sha,targets=targets)
        local['selection']='resolved_pattern_or_three_role_consensus_targets_not_zero_shift_capture'
        call_boundaries=[]
        for owner,off,target in verify.RELATIONS:
            proof=image.boundary(targets[owner],targets[owner]+off,owner)
            call_boundaries.append(dict(role=owner,site_rva=targets[owner]+off,**proof))
        marker_boundaries=[]
        for role,offset,n,digest in verify.MARKERS:
            proof=image.boundary(targets[role],targets[role]+offset,role)
            marker_boundaries.append(dict(role=role,offset=offset,**proof))
        result.update(verification=local,call_boundaries=call_boundaries,marker_boundaries=marker_boundaries,
                      method_boundary_status='PE_entry_CFG' if all(roles[k]['boundary'].get('runtime') for k in targets) else 'PARTIAL_reference_bounded_leaf_candidate')
        if local['relations_match'] and all(p['proven'] for p in call_boundaries+marker_boundaries):
            result['result']='RELOCATED_REFERENCE_RELATIONS_VERIFIED_READONLY'
        else:result['result']='SEMANTIC_OR_CALL_BOUNDARY_MISMATCH_STOP'
    if len((json.dumps(result,indent=2)+'\n').encode())>MAX_OUTPUT:raise pe.Refusal('output_cap')
    return result


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
    try:
        with a.exe.open('rb') as f:r=resolve(f.read(pe.MAX_FILE+1))
    except (OSError,ValueError,struct.error) as e:r=dict(schema=1,result='REFUSED_STOP',reason=str(e) if isinstance(e,pe.Refusal) else 'malformed_or_unavailable',production_permission=False)
    with a.output.open('x',encoding='utf-8') as f:json.dump(r,f,indent=2);f.write('\n')
    print(r['result'],a.output)
    return 0 if r['result']=='RELOCATED_REFERENCE_RELATIONS_VERIFIED_READONLY' else 2
if __name__=='__main__':raise SystemExit(main())
