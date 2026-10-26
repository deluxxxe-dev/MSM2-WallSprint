"""Bounded OFFLINE exact-slot reference candidates; never reads a live qword.

Only the pinned exe is accepted by the CLI. Decoding file bytes is not proof of
all instruction entries or indirect writers. Output never grants base/lifetime.
"""
import argparse
import hashlib
import json
import struct
from bisect import bisect_left
from collections import Counter
from pathlib import Path
import capstone as cs
from capstone import x86 as x
from capstone.x86 import X86_OP_MEM, X86_OP_REG, X86_REG_RIP, X86_REG_INVALID

EXPECTED_SHA = 'c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9'
SLOT = 0xC2D2580
DEFERRED = 0x351EB00
MAX_FILE = 512 * 1024 * 1024
MAX_SCAN = 64 * 1024 * 1024
MAX_INSNS = 5_000_000
MAX_REFS = 256

MAX_STATE_INSNS = 64
MAX_AFFINE_HOPS = 8
ANCHOR_SITE = 0x2CF7FE9
ANCHOR_ORIGIN = 0xC2D2480
FULL = [getattr(x, 'X86_REG_'+name) for name in
        ['RAX','RCX','RDX','RBX','RSP','RBP','RSI','RDI']+['R'+str(i) for i in range(8,16)]]
PARENT = {}
for bank,names in enumerate([
    ['RAX','EAX','AX','AL','AH'],['RCX','ECX','CX','CL','CH'],['RDX','EDX','DX','DL','DH'],
    ['RBX','EBX','BX','BL','BH'],['RSP','ESP','SP','SPL'],['RBP','EBP','BP','BPL'],
    ['RSI','ESI','SI','SIL'],['RDI','EDI','DI','DIL']]+[[f'R{i}',f'R{i}D',f'R{i}W',f'R{i}B'] for i in range(8,16)]):
    for name in names: PARENT[getattr(x,'X86_REG_'+name)] = bank

def decoder():
    if cs.__version__ != '5.0.6': raise ValueError('requires pinned Capstone5.0.6')
    md=cs.Cs(cs.CS_ARCH_X86,cs.CS_MODE_64);md.detail=True;return md

class AddressState:
    def __init__(self):
        self.origins=[None]*16;self.hops=[0]*16
        self.reasons=['register_base_without_exact_origin']*16;self.age=0
        self.resets=Counter()
    def reset(self,reason):
        self.origins=[None]*16;self.hops=[0]*16;self.reasons=[reason]*16;self.age=0
        self.resets[reason]+=1
    def update(self,ins,image_base,image_size,max_hops):
        ops=ins.operands;origin=None;hops=0;dest=None
        # Read source before invalidating a possibly identical destination.
        if len(ops)==2 and ops[0].type==X86_OP_REG and ops[0].reg in FULL and ops[0].size==8:
            dest=PARENT[ops[0].reg]
            if ins.id==x.X86_INS_LEA and ops[1].type==X86_OP_MEM:
                origin,_,_=resolve_address(ins,ops[1],self,image_base,image_size,1)
                hops=1 if ops[1].mem.base==X86_REG_RIP else self.hops[PARENT[ops[1].mem.base]]+1 if ops[1].mem.base in FULL else max_hops+1
            elif ins.id==x.X86_INS_MOV and ops[1].type==X86_OP_REG and ops[1].reg in FULL and ops[1].size==8:
                source=PARENT[ops[1].reg];origin=self.origins[source];hops=self.hops[source]+1
        try: _,writes=ins.regs_access()
        except cs.CsError:
            self.reset('unknown_register_effects');return
        banks={PARENT[reg] for reg in writes if reg in PARENT}
        for reg in writes:
            if reg not in PARENT: continue
            bank=PARENT[reg];self.origins[bank]=None;self.hops[bank]=0
            if reg!=FULL[bank]: reason='partial_register_mutation'
            elif len(ops)==2 and ops[1].type==X86_OP_MEM and ins.id!=x.X86_INS_LEA: reason='memory_loaded_base'
            else: reason='unsupported_register_mutation'
            self.reasons[bank]=reason
        if origin is not None and dest is not None and banks=={dest}:
            if hops<=max_hops:
                self.origins[dest]=origin;self.hops[dest]=hops;self.reasons[dest]='exact_static_image_address'
            else: self.reasons[dest]='provenance_hop_cap'

def resolve_address(ins,op,state,image_base,image_size,width):
    if op.mem.segment!=X86_REG_INVALID: return None,None,'segment_unsupported'
    if ins.addr_size!=8: return None,None,'address_size_unsupported'
    if op.mem.index!=X86_REG_INVALID: return None,None,'indexed_address'
    if op.mem.base==X86_REG_RIP:
        address=ins.address+ins.size+op.mem.disp;proof='direct_rip'
    elif op.mem.base==X86_REG_INVALID:
        address=op.mem.disp-image_base;proof='direct_absolute'
    elif op.mem.base in FULL:
        bank=PARENT[op.mem.base]
        if state.origins[bank] is None: return None,None,state.reasons[bank]
        address=state.origins[bank]+op.mem.disp;proof='register_affine_static'
    else: return None,None,'unsupported_base_register'
    # Python integers are checked before accepting x86 modulo-64 arithmetic.
    if width<=0 or address<0 or address+width>image_size or not 0<=image_base+address<=2**64-width:
        return None,None,'address_outside_image_or_wrap'
    return address,proof,None

def candidate(ins,index,op,address,proof):
    source=ins.operands[-1] if ins.operands else None
    kind=({X86_OP_REG:'register',X86_OP_MEM:'memory',x.X86_OP_IMM:'immediate'}.get(source.type,'other') if source else 'none')
    may_write=ins.id!=x.X86_INS_LEA and (bool(op.access & cs.CS_AC_WRITE) or not op.access or (index==0 and ins.id not in (x.X86_INS_CMP,x.X86_INS_TEST)))
    return dict(site_rva=ins.address,width=op.size,decoded_access=op.access,may_write=may_write,
        source_kind=kind,effective_rva=address,address_proof_class=proof,
        reference_kind='address_only' if ins.id==x.X86_INS_LEA else 'memory_operand',
        boundary='linear_sweep_candidate_not_execution_proof',
        state_scope='local_linear_block_not_global_CFG_dominance')

def is_boundary(ins):
    return any(ins.group(g) for g in [cs.CS_GRP_JUMP,cs.CS_GRP_CALL,cs.CS_GRP_RET,cs.CS_GRP_INT,cs.CS_GRP_IRET]) or ins.id in [
        x.X86_INS_SYSCALL,x.X86_INS_SYSENTER,x.X86_INS_SYSEXIT,x.X86_INS_SYSRET,x.X86_INS_UD2]

def scan_code(code,rva,slot=SLOT,image_base=0x140000000,
              max_bytes=MAX_SCAN,max_insns=MAX_INSNS,max_refs=MAX_REFS,
              image_size=0xD000000,reset_points=(),max_state_insns=MAX_STATE_INSNS,max_hops=MAX_AFFINE_HOPS):
    """Bounded local address candidates, not global execution/dominance proof."""
    md=decoder();state=AddressState();reasons=Counter()
    result=dict(bytes_scanned=0,instructions=0,decode_gaps=0,capped=False,references=[],
                unresolved_memory_operands=0,register_affine_resolved_operands=0)
    cursor=0;limit=min(len(code),max_bytes)
    points=sorted(set(reset_points));point=bisect_left(points,rva)
    while cursor<limit and result['instructions']<max_insns:
        start=cursor
        for ins in md.disasm(code[cursor:min(limit,cursor+65536+15)],rva+cursor):
            if cursor-start>=65536: break
            if state.age>=max_state_insns: state.reset('provenance_window_exhausted')
            while point<len(points) and points[point]<=ins.address:
                state.reset('runtime_region_boundary_loss');point+=1
            result['instructions']+=1;state.age+=1
            for index,op in enumerate(ins.operands):
                if op.type!=X86_OP_MEM: continue
                address,proof,reason=resolve_address(ins,op,state,image_base,image_size,op.size)
                if reason:
                    result['unresolved_memory_operands']+=1;reasons[reason]+=1;continue
                if proof=='register_affine_static': result['register_affine_resolved_operands']+=1
                if address<slot+8 and slot<address+op.size:
                    if len(result['references'])>=max_refs:
                        result['capped']=True;result['bytes_scanned']=ins.address+ins.size-rva
                        result['unresolved_reasons']=dict(reasons);result['state_resets']=dict(state.resets);return result
                    result['references'].append(candidate(ins,index,op,address,proof))
            state.update(ins,image_base,image_size,max_hops)
            if is_boundary(ins): state.reset('control_flow_boundary_loss')
            cursor=ins.address+ins.size-rva
            if result['instructions']>=max_insns: break
        if cursor==start:
            cursor+=1;result['decode_gaps']+=1;state.reset('decode_gap')
    result['bytes_scanned']=cursor;result['capped']|=cursor<len(code)
    result['unresolved_reasons']=dict(reasons);result['state_resets']=dict(state.resets)
    return result

def external_anchor_record(code,image_base,image_size):
    """Only called after exact image authentication; NOT a local sweep result.
    Current instruction plus the accepted c28 parent-origin evidence are required.
    The origin is not loaded/dereferenced and is never injected into sweep state.
    """
    ins=next(decoder().disasm(code,ANCHOR_SITE,count=1),None)
    if not ins or ins.id!=x.X86_INS_MOV or ins.size!=7 or len(ins.operands)!=2: return None
    dest,op=ins.operands
    if not (dest.type==X86_OP_REG and dest.reg==x.X86_REG_RAX and dest.size==8 and dest.access==cs.CS_AC_WRITE and
        op.type==X86_OP_MEM and op.size==8 and op.access==cs.CS_AC_READ and ins.addr_size==8 and
        op.mem.base==x.X86_REG_RCX and op.mem.index==X86_REG_INVALID and op.mem.segment==X86_REG_INVALID and
        op.mem.disp==0x100): return None
    state=AddressState();state.origins[PARENT[x.X86_REG_RCX]]=ANCHOR_ORIGIN
    address,_,reason=resolve_address(ins,op,state,image_base,image_size,8)
    if reason or address!=SLOT: return None
    record=candidate(ins,1,op,address,'authenticated_external_anchor')
    record.update(evidence='accepted_c28_runtime_parent_origin_plus_current_image_instruction',
        boundary='exact_image_current_instruction_plus_accepted_runtime_site_not_local_sweep',
        origin_root=16,origin_rva=ANCHOR_ORIGIN,local_scanner_coverage=False,
        state_scope='authenticated_parent_fact_not_locally_derived_callee_entry')
    return record

def publication_verdict(scans,coverage_complete=False):
    refs=[ref for scan in scans for ref in scan['references']]
    writers=[ref for ref in refs if ref['may_write']]
    direct=[ref for ref in writers if ref['address_proof_class'] in ('direct_rip','direct_absolute')]
    affine=[ref for ref in writers if ref['address_proof_class']=='register_affine_static']
    reason=('multiple_writer_candidates_ambiguous' if len(writers)>1 else
            'indirect_writers_and_publication_lifetime_unproven' if coverage_complete else
            'xref_coverage_incomplete_indirect_writers_unexcluded')
    return dict(slot_rva=SLOT,reference_candidates=refs,direct_writer_candidates=len(direct),
        register_affine_writer_candidates=len(affine),writer_candidates=len(writers),
        selected_byte_sweep_complete=coverage_complete,candidate_sweep_complete=False,
        writer_enumeration_complete=False,publication_source_kind='unproven',allocation_proven=False,
        range_proven=False,lifetime_proven=False,base_proven=False,destination_contained=False,CFG_continuation=False,
        live_qword_reads=0,pointer_chase=False,reason=reason)

def parse_pe(data):
    def u16(at): return struct.unpack_from('<H', data, at)[0]
    def u32(at): return struct.unpack_from('<I', data, at)[0]
    if len(data) < 64 or data[:2] != b'MZ': raise ValueError('invalid DOS header')
    pe = u32(60)
    if pe > len(data)-264 or data[pe:pe+4] != b'PE\0\0' or u16(pe+4) != 0x8664:
        raise ValueError('invalid x64 PE')
    optional = pe+24; count=u16(pe+6); size=u16(pe+20)
    if not 0 < count <= 96 or size < 240 or u16(optional) != 0x20b:
        raise ValueError('invalid sections/PE32+')
    image_size=u32(optional+56); image_base=struct.unpack_from('<Q',data,optional+24)[0]
    sections=[]
    for i in range(count):
        at=optional+size+40*i
        if at+40 > len(data): raise ValueError('truncated sections')
        virtual=u32(at+8); begin=u32(at+12); raw_size=u32(at+16); raw=u32(at+20)
        if begin > image_size or virtual > image_size-begin or raw > len(data) or raw_size > len(data)-raw:
            raise ValueError('section outside image/file')
        end=begin+virtual
        if any(begin < e and b < end for b,e,_,_,_ in sections): raise ValueError('overlapping sections')
        sections.append((begin,end,raw,raw_size,u32(at+36)))
    def file_range(rva,n):
        for begin,end,raw,raw_size,_ in sections:
            if begin <= rva and n <= end-rva and rva-begin+n <= raw_size:
                return data[raw+rva-begin:raw+rva-begin+n]
        raise ValueError('metadata range not file backed')
    excluded=[]
    table=u32(optional+112+3*8); table_size=u32(optional+116+3*8)
    if bool(table) != bool(table_size) or table_size%12 or table_size > 8*1024*1024 or table_size//12 > 600000:
        raise ValueError('runtime table invalid/capped')
    rows=file_range(table,table_size) if table_size else b''
    previous=0;reset_points=set()
    for at in range(0,len(rows),12):
        begin,end,unwind=struct.unpack_from('<III',rows,at)
        if begin < previous or begin >= end or end > image_size or not 0 < unwind < image_size:
            raise ValueError('runtime table malformed')
        previous=end;reset_points.update((begin,end))
        if begin <= DEFERRED < end: excluded.append((begin,end))
    if not excluded:
        excluded=[(b,e) for b,e,_,_,flags in sections if b <= DEFERRED < e and flags & 0x20000000]
    return sections,image_base,excluded,file_range,image_size,reset_points

def analyze_image(data):
    if len(data) > MAX_FILE: raise ValueError('input file cap')
    if hashlib.sha256(data).hexdigest() != EXPECTED_SHA: raise ValueError('exact executable SHA256 mismatch')
    if cs.__version__ != '5.0.6': raise ValueError('requires pinned Capstone5.0.6')
    sections,base,excluded,file_range,image_size,reset_points=parse_pe(data)
    slot_sections=[s for s in sections if s[0] <= SLOT and SLOT+8 <= s[1]]
    if len(slot_sections)!=1: raise ValueError('slot section not unique')
    # Reserve all anchor decoding/record work within the original aggregate caps.
    anchors=[];anchor_bytes=anchor_insns=0;anchor_status='current_instruction_unavailable_or_unmatched'
    if not any(b<=ANCHOR_SITE<e for b,e in excluded):
        code_regions=[s for s in sections if s[0]<=ANCHOR_SITE and ANCHOR_SITE+7<=s[1] and (s[4]&0x60000000)==0x60000000 and not s[4]&0x80000000]
        if len(code_regions)==1:
            try: anchor_code=file_range(ANCHOR_SITE,7)
            except ValueError: anchor_code=None
            if anchor_code is not None:
                anchor_bytes=7;anchor_insns=1
                record=external_anchor_record(anchor_code,base,image_size)
                if record: anchors.append(record);anchor_status='accepted_parent_anchor_current_instruction_verified_not_local_coverage'
    else: anchor_status='anchor_within_excluded_region_not_decoded'
    scans=[];budget=MAX_SCAN-anchor_bytes;insns=MAX_INSNS-anchor_insns;refs=MAX_REFS-len(anchors)
    for begin,end,raw,raw_size,flags in sections:
        if not flags & 0x20000000: continue
        spans=[(begin,min(end,begin+raw_size))]
        for low,high in excluded:
            spans=[part for b,e in spans for part in [(b,min(e,low)),(max(b,high),e)] if part[0]<part[1]]
        for low,high in spans:
            if budget <= 0 or insns <= 0 or refs <= 0: break
            scan=scan_code(data[raw+low-begin:raw+high-begin],low,image_base=base,
                           max_bytes=budget,max_insns=insns,max_refs=refs,image_size=image_size,reset_points=reset_points)
            scans.append(scan);budget-=scan['bytes_scanned'];insns-=scan['instructions'];refs-=len(scan['references'])
    complete=not excluded and budget>0 and insns>0 and refs>0 and all(not s['capped'] and not s['decode_gaps'] for s in scans)
    result=publication_verdict(scans,complete)
    result.update(image_authenticated=True, tool_version='v0.1c38', schema_version=2, scan_scope='bounded_offline_direct_and_register_affine_candidates_not_all_writes',
        slot_metadata=dict(readable=bool(slot_sections[0][4]&0x40000000),writable=bool(slot_sections[0][4]&0x80000000)),
        excluded_deferred_regions=excluded, cap_bytes=MAX_SCAN, cap_instructions=MAX_INSNS,
        bytes_scanned=sum(s['bytes_scanned'] for s in scans)+anchor_bytes, decode_gaps=sum(s['decode_gaps'] for s in scans),
        indirect_or_unresolved_operands=sum(s['unresolved_memory_operands'] for s in scans),
        unresolved_reasons=dict(sum((Counter(s['unresolved_reasons']) for s in scans),Counter())),
        register_affine_resolved_operands=sum(s['register_affine_resolved_operands'] for s in scans),
        state_resets=dict(sum((Counter(s['state_resets']) for s in scans),Counter())),
        cap_state_instructions=MAX_STATE_INSNS,cap_affine_hops=MAX_AFFINE_HOPS,
        external_anchors=anchors,external_anchor_status=anchor_status,
        anchor_bytes_within_total_cap=anchor_bytes,anchor_instructions_within_total_cap=anchor_insns,
        instructions_decoded=sum(s['instructions'] for s in scans)+anchor_insns,
        anchor_counted_as_local_candidate=False)
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    if args.exe.stat().st_size > MAX_FILE: parser.error('input file cap')
    try: result=analyze_image(args.exe.read_bytes())
    except (ValueError,struct.error) as exc: parser.error(str(exc))
    args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print('PASS: authenticated exact-slot candidate metadata written; base/containment remain unproven')
if __name__ == '__main__': main()
